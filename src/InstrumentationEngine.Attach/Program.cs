// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Xml;
using System.Xml.Serialization;
using Microsoft.Diagnostics.NETCore.Client;
using static System.FormattableString;

namespace Microsoft.InstrumentationEngine
{
    internal enum Chip
    {
        x86,
        x64
    }

    internal sealed class Program
    {
        private static readonly Guid InstrumentationEngineClsid = new Guid("324F817A-7420-4E6D-B3C1-143FBED6D855");

        private const int ExitCodeSuccess = 0;
        private const int ExitCodeFailure = -1;

        private const string Instrumentation32FolderName = "Instrumentation32";
        private const string Instrumentation64FolderName = "Instrumentation64";

        private const string InstrumentationEngineX64LinuxName = "libInstrumentationEngine.so";
        private const string InstrumentationEngineX64WindowsName = "MicrosoftInstrumentationEngine_x64.dll";
        private const string InstrumentationEngineX86WindowsName = "MicrosoftInstrumentationEngine_x86.dll";

        internal static int Main(string[] args)
        {
            if (args.Length != 2)
            {
                WriteError("Expected two arguments:");
                WriteError("\t<pid>        - process ID to which the engine shall be attached.");
                WriteError("\t<config.xml> - configuration xml for Instrumentation Methods.");
                return ExitCodeFailure;
            }

            #region Parameters
            string processIdString = args[0];

            if (!Int32.TryParse(processIdString, out int processId))
            {
                WriteError(Invariant($"Could not parse process ID argument '{processIdString}' to integer."));
                return ExitCodeFailure;
            }

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

            // Attempt to resolve fullpath
            string configFilePath = Path.GetFullPath(args[1]);
            if (!File.Exists(configFilePath))
            {
                WriteError(Invariant($"Could not find file '{configFilePath}'."));
                return ExitCodeFailure;
            }

            #endregion

            #region Parse Configuration Sources
            InstrumentationConfigurationSources? configSources = null;

            // Schema file embedded name is the same that the full type name that was generated from the schema file
            string schemaResourceName = typeof(InstrumentationConfigurationSources).FullName + ".xsd";

            // Read schema file
            using (Stream? schemaStream = Assembly.GetExecutingAssembly().GetManifestResourceStream(schemaResourceName))
            using (XmlReader schemaReader = XmlReader.Create(schemaStream, new XmlReaderSettings() { XmlResolver = null }))
            {
                // Create reader settings that validate description file
                XmlReaderSettings readerSettings = new XmlReaderSettings();
                readerSettings.Schemas.Add(string.Empty, schemaReader);
                readerSettings.ValidationType = ValidationType.Schema;

                // Read description file
                using (XmlReader reader = XmlReader.Create(configFilePath, readerSettings))
                {
                    XmlSerializer serializer = new XmlSerializer(typeof(InstrumentationConfigurationSources));
                    try
                    {
                        configSources = (InstrumentationConfigurationSources)serializer.Deserialize(reader);
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

            #region RootPath
                        // This executable should be in the [Root]\Tools\Attach directory. Get the root directory
                        // and build back to the instrumentation engine file path.

                        // CONSIDER: Is there a better way to get the engine path instead of building the path
                        // relative to the current executable? For example, requiring the root path of the engine
                        // to be passed as a parameter (which increases the difficulty of using this executable).

                        string ? attachDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
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

            string? rootDirectory = Path.GetDirectoryName(toolsDirectory);
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
            // Create CLRIE configuration object for XML serialziation
            if (!TryParseEngineConfiguration(
                    configSources,
                    isTargetProcess32Bit ? Chip.x86 : Chip.x64,
                    out InstrumentationEngineConfiguration engineConfig))
            {
                WriteError("Unable to parse Engine Configuration from configuration sources.");
                return ExitCodeFailure;
            }


            XmlSerializer ser = new XmlSerializer(
                typeof(InstrumentationEngineConfiguration),
                new Type[]
                {
                    typeof(InstrumentationMethod),
                    typeof(AddInstrumentationMethod)
                });
            MemoryStream m = new MemoryStream();
            ser.Serialize(m, engineConfig);

            // reset to 0 so we start reading from the beginning of the stream
            m.Position = 0;
            byte[] bytes = System.Text.Encoding.Unicode.GetBytes(new StreamReader(m).ReadToEnd());

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

        private static bool TryParseEngineConfiguration(
            InstrumentationConfigurationSources sources,
            Chip targetChip,
            out InstrumentationEngineConfiguration configuration)
        {
            configuration = new InstrumentationEngineConfiguration();
            configuration.InstrumentationEngine = new InstrumentationEngine()
            {
                Settings = new Setting[]
                {
                    new Setting()
                    {
                        Name="LogLevel",
                        Value="Errors"
                    }
                }
            };

            var methodsList = new List<InstrumentationMethod>();
            foreach (var source in sources.InstrumentationConfigurationSource)
            {
                var addMethodList = new List<AddInstrumentationMethod>(sources.InstrumentationConfigurationSource.Length);
                foreach (var platform in source.Platforms)
                {
                    AddInstrumentationMethod method;
                    if (platform.Chip.Equals(targetChip.ToString("G"), StringComparison.OrdinalIgnoreCase))
                    {
                        method = new AddInstrumentationMethod()
                        {
                            ConfigPath = platform.Path
                        };

                        List<Setting> settingsList = new List<Setting>(source.Settings.Length);
                        foreach (var setting in source.Settings)
                        {
                            settingsList.Add(new Setting()
                            {
                                Name = setting.Name,
                                Value = setting.Value
                            });

                        }
                        method.Settings = settingsList.ToArray();
                        addMethodList.Add(method);
                    }
                    else
                    {
                        continue;
                    }
                }
                methodsList.AddRange(addMethodList);
            }

            configuration.InstrumentationMethods = methodsList.ToArray();
            return true;
        }
    }
}
