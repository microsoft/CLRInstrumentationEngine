// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.Diagnostics.NETCore.Client;
using System;
using System.Collections.Generic;
using System.CommandLine;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Xml;
using System.Xml.Serialization;
using static System.FormattableString;

namespace Microsoft.InstrumentationEngine
{
    /// <summary>
    /// Intermediate class used to track ConfigurationSources when generating an EngineConfiguration.
    /// </summary>
    internal class ConfigurationSourceInfo
    {
        public InstrumentationConfigurationSources? Sources { get; set; }

        public string? ConfigSourceDirectory { get; set; }

        public string? ConfigSourceFilePath { get; set; }
    }

    [Flags]
    internal enum LoggingFlags
    {
        None = 0,
        Errors = 1,
        Messages = 2,
        Dumps = 4
    }

    internal class AttachCommandHandler
    {
        #region Fields
        private static readonly Guid InstrumentationEngineClsid = new Guid("324F817A-7420-4E6D-B3C1-143FBED6D855");

        private const int ExitCodeSuccess = 0;
        private const int ExitCodeFailure = -1;

        private const string Instrumentation32FolderName = "Instrumentation32";
        private const string Instrumentation64FolderName = "Instrumentation64";

        private const string InstrumentationEngineX64LinuxName = "libInstrumentationEngine.so";
        private const string InstrumentationEngineX64WindowsName = "MicrosoftInstrumentationEngine_x64.dll";
        private const string InstrumentationEngineX86WindowsName = "MicrosoftInstrumentationEngine_x86.dll";
        #endregion

        #region Methods
        private static int Attach(int processId,
            FileInfo[] configs,
            LoggingFlags logLevel,
            LoggingFlags logFileLevel,
            FileSystemInfo? logFilePath)
        {
            #region Target Process
            Process process;
            try
            {
                process = Process.GetProcessById(processId);
            }
            catch (Exception ex)
            {
                WriteError(ex.Message);
                return ExitCodeFailure;
            }
            #endregion

            #region Configuration Sources
            var sourceInfos = new List<ConfigurationSourceInfo>();

            // Schema file embedded name is the same that the full type name that was generated from the schema file
            string schemaResourceName = typeof(InstrumentationConfigurationSources).FullName + ".xsd";

            foreach (var configFile in configs)
            {
                var sourceInfo = new ConfigurationSourceInfo()
                {
                    ConfigSourceFilePath = configFile.FullName
                };

                // Attempt to resolve fullpath
                string? configFileDirectory = Path.GetDirectoryName(sourceInfo.ConfigSourceFilePath);
                if (null == configFileDirectory || !Directory.Exists(configFileDirectory))
                {
                    WriteError(Invariant($"Could not find directory '{configFileDirectory}'."));
                    return ExitCodeFailure;
                }

                sourceInfo.ConfigSourceDirectory = configFileDirectory;

                #region Parse Configuration Sources

                // Read schema file
                using (Stream? schemaStream = Assembly.GetExecutingAssembly().GetManifestResourceStream(schemaResourceName))
                using (XmlReader schemaReader = XmlReader.Create(schemaStream, new XmlReaderSettings() { XmlResolver = null, DtdProcessing = DtdProcessing.Prohibit }))
                {
                    // Create reader settings that conducts validation
                    XmlReaderSettings readerSettings = new XmlReaderSettings() { XmlResolver = null, DtdProcessing = DtdProcessing.Prohibit };
                    readerSettings.Schemas.Add(string.Empty, schemaReader);
                    readerSettings.ValidationType = ValidationType.Schema;

                    // Read configuration source file
                    using (XmlReader reader = XmlReader.Create(sourceInfo.ConfigSourceFilePath, readerSettings))
                    {
                        XmlSerializer serializer = new XmlSerializer(typeof(InstrumentationConfigurationSources));
                        try
                        {
                            sourceInfo.Sources = (InstrumentationConfigurationSources)serializer.Deserialize(reader);
                        }
                        // InvalidOperationException is thrown when XML does not conform to the schema
                        catch (InvalidOperationException ex)
                        {
                            // Log the location of the error
                            WriteError($"Error: {ex.Message}");
                            if (null != ex.InnerException)
                            {
                                // Log the detailed information of why the XML does not conform to the schema
                                WriteError(ex.InnerException.Message);
                            }
                            return ExitCodeFailure;
                        }
                    }
                }

                #endregion

                sourceInfos.Add(sourceInfo);
            }
            #endregion

            #region Engine RootPath
            // This executable should be in the [Root]\Tools\Attach directory. Get the root directory
            // and build back to the instrumentation engine file path.

            // CONSIDER: Is there a better way to get the engine path instead of building the path
            // relative to the current executable? For example, requiring the root path of the engine
            // to be passed as a parameter (which increases the difficulty of using this executable).

            string? rootDirectory = Environment.GetEnvironmentVariable("MicrosoftInstrumentationEngine_InstallationRoot");
            if (string.IsNullOrEmpty(rootDirectory))
            {
                string? attachDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
                if (null == attachDirectory || !Directory.Exists(attachDirectory))
                {
                    WriteError(Invariant($"Directory '{attachDirectory}' does not exist."));
                    return ExitCodeFailure;
                }

                string? toolsDirectory = Path.GetDirectoryName(attachDirectory);
                if (null == toolsDirectory || !Directory.Exists(toolsDirectory))
                {
                    WriteError(Invariant($"Directory '{toolsDirectory}' does not exist."));
                    return ExitCodeFailure;
                }

                rootDirectory = Path.GetDirectoryName(toolsDirectory);
            }

            if (null == rootDirectory || !Directory.Exists(rootDirectory))
            {
                WriteError(Invariant($"Directory '{rootDirectory}' does not exist."));
                return ExitCodeFailure;
            }
            #endregion

            #region Target Process Architecture
            bool isTargetProcess32Bit = true;
            if (RuntimeInformation.OSArchitecture == Architecture.X64)
            {
                isTargetProcess32Bit = false;
                if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
                {
                    try
                    {
                        // Returns true for 32-bit applications running on a 64-bit OS
                        if (!NativeMethods.IsWow64Process(process.Handle, out isTargetProcess32Bit))
                        {
                            Exception? ex = Marshal.GetExceptionForHR(Marshal.GetHRForLastWin32Error());
                            if (null != ex)
                            {
                                WriteError(Invariant($"Failed to check process bitness: {ex.Message}"));
                            }
                            else
                            {
                                WriteError(Invariant($"Failed to check process bitness."));
                            }
                            return ExitCodeFailure;
                        }
                    }
                    catch (Win32Exception ex)
                    {
                        WriteError(Invariant($"Failed to check process bitness (code: {ex.NativeErrorCode}): {ex.Message}"));
                        return ExitCodeFailure;
                    }
                }
            }
            #endregion

            #region Generate Engine Configuration
            // Create CLRIE configuration object for XML serialization
            var configuration = new InstrumentationEngineConfiguration();

            configuration.InstrumentationEngine = GenerateEngineConfiguration(
                logLevel,
                logFileLevel,
                logFilePath);

            if (TryParseInstrumentationMethodConfiguration(
                    sourceInfos,
                    isTargetProcess32Bit ? ChipType.x86 : ChipType.x64,
                    out InstrumentationMethodsTypeAddInstrumentationMethod[]? methods) &&
                methods != null)
            {
                configuration.InstrumentationMethods = methods;
            }
            else
            {
                WriteError("Unable to parse Instrumentation Methods from configuration sources.");
                return ExitCodeFailure;
            }

            XmlSerializer engineConfigSerializer = new XmlSerializer(typeof(InstrumentationEngineConfiguration));
            MemoryStream memStream = new MemoryStream();
            engineConfigSerializer.Serialize(memStream, configuration);
            byte[] bytes = memStream.ToArray();
            #endregion

            #region InstrumentationEngine Path
            string enginePath;
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                enginePath = isTargetProcess32Bit ?
                    Path.Combine(rootDirectory, Instrumentation32FolderName, InstrumentationEngineX86WindowsName) :
                    Path.Combine(rootDirectory, Instrumentation64FolderName, InstrumentationEngineX64WindowsName);
            }
            else if (!isTargetProcess32Bit)
            {
                enginePath = Path.Combine(rootDirectory, Instrumentation64FolderName, InstrumentationEngineX64LinuxName);
            }
            else
            {
                WriteError("Only the 64 bit engine is supported on non-Windows platforms.");
                return ExitCodeFailure;
            }

            if (!File.Exists(enginePath))
            {
                WriteError(Invariant($"Engine path '{enginePath}' does not exist."));
                return ExitCodeFailure;
            }
            #endregion

            #region Profiler Attach
            // CONSIDER: Should the engine be validated before attempting to attach it to the CLR?
            // For example, could check the signature on the assembly (which wouldn't work on Linux).
            DiagnosticsClient client = new DiagnosticsClient(processId);
            try
            {
                client.AttachProfiler(TimeSpan.FromSeconds(10), InstrumentationEngineClsid, enginePath, bytes);
            }
            catch (ServerErrorException ex)
            {
                WriteError(Invariant($"Could not attach engine to process: '{ex.Message}'."));
                return ExitCodeFailure;
            }
            #endregion

            return ExitCodeSuccess;
        }

        private static void WriteError(string message)
        {
            Console.Error.WriteLine(message);
        }

        private static InstrumentationEngineConfigurationInstrumentationEngine GenerateEngineConfiguration(
            LoggingFlags logLevel,
            LoggingFlags logFileLevel,
            FileSystemInfo? logFilePath)
        {
            var engine = new InstrumentationEngineConfigurationInstrumentationEngine();

            // Parse engine settings
            var engineSettings = new List<SettingsTypeSetting>();

            engineSettings.Add(
                new SettingsTypeSetting()
                {
                    Name = "LogLevel",
                    Value = logLevel.ToString("G").Replace(',', '|')
                });

            engineSettings.Add(
                new SettingsTypeSetting()
                {
                    Name = "LogFileLevel",
                    Value = logFileLevel.ToString("G").Replace(',', '|')
                });

            if (logFilePath != null)
            {
                engineSettings.Add(
                    new SettingsTypeSetting()
                    {
                        Name = "LogFilePath",
                        Value = logFilePath.FullName
                    });
            }

            engine.Settings = engineSettings.ToArray();
            return engine;
        }

        /// <summary>
        /// This function iterates through each <see cref="ConfigurationSourceInfo" /> in order
        /// to generate an <see cref="InstrumentationMethodsTypeAddInstrumentationMethod" /> objects.
        /// </summary>
        private static bool TryParseInstrumentationMethodConfiguration(
            List<ConfigurationSourceInfo> sourceInfos,
            ChipType targetChip,
            out InstrumentationMethodsTypeAddInstrumentationMethod[]? methods)
        {
            methods = null;
            var methodsSection = new List<InstrumentationMethodsTypeAddInstrumentationMethod>();

            // Parse methods section
            if (sourceInfos == null || sourceInfos.Count == 0)
            {
                // Short-cirtcuit here, we don't have sources to parse.
                WriteError("Missing configuration sources.");
                return false;
            }

            foreach (var sourceInfo in sourceInfos)
            {
                if (sourceInfo.Sources == null ||
                    sourceInfo.ConfigSourceDirectory == null)
                {
                    continue;
                }

                if (sourceInfo.Sources.InstrumentationConfigurationSource != null)
                {
                    foreach (var source in sourceInfo.Sources.InstrumentationConfigurationSource)
                    {
                        if (source.Platforms != null)
                        {
                            foreach (var platform in source.Platforms)
                            {
                                if (platform.Chip.Equals(targetChip))
                                {
                                    string configFullPath = Path.GetFullPath(Path.Combine(sourceInfo.ConfigSourceDirectory, platform.Path));
                                    var methodSettings = new List<SettingsTypeSetting>();
                                    var methodSection = new InstrumentationMethodsTypeAddInstrumentationMethod();
                                    if (File.Exists(configFullPath))
                                    {
                                        methodSection.ConfigPath = configFullPath;
                                    }
                                    else
                                    {
                                        WriteError($"Instrumentation Method Config file was not found at {configFullPath}.");
                                        return false;
                                    }

                                    // Allow methods without any settings.
                                    if (source.Settings != null)
                                    {
                                        foreach (var setting in source.Settings)
                                        {
                                            string settingValue;
                                            if (setting.IsPathSpecified &&
                                                setting.IsPath)
                                            {
                                                // Assumes path is relative, however Path.Combine will ignore all arguments left of one that is absolute.
                                                settingValue = Path.Combine(sourceInfo.ConfigSourceDirectory, setting.Value);
                                            }
                                            else
                                            {
                                                settingValue = setting.Value;
                                            }

                                            methodSettings.Add(new SettingsTypeSetting()
                                            {
                                                Name = setting.Name,
                                                Value = settingValue
                                            });
                                        }

                                        methodSection.Settings = methodSettings.ToArray();
                                    }

                                    methodsSection.Add(methodSection);
                                }
                                else
                                {
                                    // Mismatch of platform, skipping.
                                    continue;
                                }
                            }
                        }
                        else
                        {
                            WriteError($"No platforms found for ConfigurationSource '{sourceInfo.ConfigSourceFilePath}'.");
                            return false;
                        }
                    }
                }
                else
                {
                    WriteError($"No ConfigurationSources provided for '{sourceInfo.ConfigSourceFilePath}'.");
                    return false;
                }
            }

            methods = methodsSection.ToArray();
            return true;
        }
        #endregion

        #region System.CommandLine
        delegate int AttachDelegate(
            int processId,
            FileInfo[] configs,
            LoggingFlags logLevel,
            LoggingFlags logFileLevel,
            FileSystemInfo logFilePath);

        /// <summary>
        /// This function generates the subcommand with options.
        /// </summary>
        public static Command GetCommand()
        {
            var command = new Command(
                name: "attach",
                description: "Attaches CLR Instrumentation Engine to a running process.")
            {
                ProcessIdOption(),
                ConfigFileOption(),
                EngineLogLevelOption(),
                EngineLogFileLevelOption(),
                EngineLogFilePathOption()
            };

            command.Handler = System.CommandLine.Invocation.CommandHandler.Create((AttachDelegate)Attach);
            return command;
        }

        /// <summary>
        /// CommandLine Option for Process Id
        /// </summary>
        private static Option ProcessIdOption() =>
            new Option<int>(
                aliases: new[] { "--process-id", "-p" },
                description: "The target process id to attach.")
            {
                Argument = new Argument<int>(name: "process-id"),
                Required = true
            };

        /// <summary>
        /// CommandLine Option for Config File
        /// </summary>
        private static Option ConfigFileOption() =>
            new Option(
                aliases: new[] { "--configs", "-c" },
                description: "Path to configuration source xml files.")
            {
                Argument = new Argument<FileInfo[]>().ExistingOnly(),
                Required = true
            };

        /// <summary>
        /// CommandLine Option for Engine LogLevel
        /// </summary>
        /// <returns></returns>
        private static Option EngineLogLevelOption() =>
            new Option(
                aliases: new[] { "--log-level" },
                description: "The overall log level used by all engine log sinks. Use comma to delimit multiple values.")
            {
                Argument = new Argument<LoggingFlags>(),
                Required = false
            };

        /// <summary>
        /// CommandLine Option for Engine FileLog Level
        /// </summary>
        /// <returns></returns>
        private static Option EngineLogFileLevelOption() =>
            new Option(
                aliases: new[] { "--log-file-level" },
                description: "The log level for the engine log file sink. Use comma to delimit multiple values.")
            {
                Argument = new Argument<LoggingFlags>(),
                Required = false
            };

        /// <summary>
        /// CommandLine Option for Engine FileLog Path
        /// </summary>
        /// <returns></returns>
        private static Option EngineLogFilePathOption() =>
            new Option(
                aliases: new[] { "--log-file-path" },
                description: "The log file path for the engine log file sink.")
            {
                Argument = new Argument<FileSystemInfo>().LegalFilePathsOnly(),
                Required = false
            };
        #endregion
    }
}
