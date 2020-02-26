// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using Microsoft.Diagnostics.NETCore.Client;
using static System.FormattableString;

namespace Microsoft.InstrumentationEngine
{
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
            // TODO: Parse command line for additional arguments such as configuration files
            if (args.Length != 1)
            {
                WriteError("Expected single argument that represents the process ID to which the engine shall be attached.");
                return ExitCodeFailure;
            }

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

            // This executable should be in the [Root]\Tools\Attach directory. Get the root directory
            // and build back to the instrumentation engine file path.

            // CONSIDER: Is there a better way to get the engine path instead of building the path
            // relative to the current executable? For example, requiring the root path of the engine
            // to be passed as a parameter (which increases the difficulty of using this executable).

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

            string? rootDirectory = Path.GetDirectoryName(toolsDirectory);
            if (null == rootDirectory || !Directory.Exists(rootDirectory))
            {
                WriteError(Invariant($"Directory '{rootDirectory}' does not exist."));
                return ExitCodeFailure;
            }

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

            // CONSIDER: Should the engine be validated before attempting to attach it to the CLR?
            // For example, could check the signature on the assembly (which wouldn't work on Linux).

            DiagnosticsClient client = new DiagnosticsClient(processId);
            try
            {
                client.AttachProfiler(TimeSpan.FromSeconds(10), InstrumentationEngineClsid, enginePath);
            }
            catch (ServerErrorException ex)
            {
                WriteError(Invariant($"Could not attach engine to process: '{ex.Message}'."));
                return ExitCodeFailure;
            }

            return ExitCodeSuccess;
        }

        private static void WriteError(string message)
        {
            Console.Error.WriteLine(message);
        }
    }
}
