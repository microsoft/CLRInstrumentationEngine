// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.Json;
using Microsoft.Diagnostics.NETCore.Client;
using static System.FormattableString;

namespace Microsoft.InstrumentationEngine
{
    internal sealed class Program
    {
        private static readonly Guid InstrumentationEngineClsid = new Guid("324F817A-7420-4E6D-B3C1-143FBED6D855");

        internal static void Main(string[] args)
        {
            // TODO: Parse command line for additional arguments such as configuration files
            if (args.Length != 1)
            {
                WriteErrorJson("Expected single argument that represents the process ID to which the engine shall be attached.");
                return;
            }

            string processIdString = args[0];

            if (!Int32.TryParse(processIdString, out int processId))
            {
                WriteErrorJson(Invariant($"Could not parse process ID argument '{processIdString}' to integer."));
                return;
            }

            Process process;
            try
            {
                process = Process.GetProcessById(processId);
            }
            catch (Exception ex)
            {
                WriteErrorJson(ex.Message);
                return;
            }

            // This executable should be in the [Root]\Tools\Attach directory. Get the root directory
            // and build back to the instrumentation engine file path.

            // CONSIDER: Is there a better way to get the engine path instead of building the path
            // relative to the current executable? For example, requiring the root path of the engine
            // to be passed as a parameter (which increases the difficulty of using this executable).

            string? attachDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            if (null == attachDirectory || !Directory.Exists(attachDirectory))
            {
                WriteErrorJson(Invariant($"Directory '{attachDirectory}' does not exist."));
                return;
            }

            string? toolsDirectory = Path.GetDirectoryName(attachDirectory);
            if (null == toolsDirectory || !Directory.Exists(toolsDirectory))
            {
                WriteErrorJson(Invariant($"Directory '{toolsDirectory}' does not exist."));
                return;
            }

            string? rootDirectory = Path.GetDirectoryName(toolsDirectory);
            if (null == rootDirectory || !Directory.Exists(rootDirectory))
            {
                WriteErrorJson(Invariant($"Directory '{rootDirectory}' does not exist."));
                return;
            }

            string enginePath;
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            {
                enginePath = Path.Combine(rootDirectory, "Instrumentation64", "libInstrumentationEngine.so");
            }
            else
            {
                bool isWow64;
                try
                {
                    // isWow64 is true for 32-bit applications running on a 64-bit OS
                    if (!NativeMethods.IsWow64Process(process.Handle, out isWow64))
                    {
                        Exception? ex = Marshal.GetExceptionForHR(Marshal.GetHRForLastWin32Error());
                        if (null != ex)
                        {
                            WriteErrorJson(Invariant($"Failed to check process bitness: {ex.Message}"));
                        }
                        else
                        {
                            WriteErrorJson(Invariant($"Failed to check process bitness."));
                        }
                    }
                }
                catch (Win32Exception ex)
                {
                    WriteErrorJson(Invariant($"Failed to check process bitness (code: {ex.NativeErrorCode}): {ex.Message}"));
                    return;
                }

                enginePath = isWow64 ?
                    Path.Combine(rootDirectory, "Instrumentation32", "MicrosoftInstrumentationEngine_x86.dll") :
                    Path.Combine(rootDirectory, "Instrumentation64", "MicrosoftInstrumentationEngine_x64.dll");
            }

            if (!File.Exists(enginePath))
            {
                WriteErrorJson(Invariant($"Engine path '{enginePath}' does not exist."));
                return;
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
                WriteErrorJson(Invariant($"Could not attach engine to process: '{ex.Message}'."));
                return;
            }

            WriteSuccessJson();
        }

        private static void WriteErrorJson(string message)
        {
            Console.WriteLine(CreateResultJson("Failed", message));
        }

        private static void WriteSuccessJson()
        {
            Console.WriteLine(CreateResultJson("Success"));
        }

        private static string CreateResultJson(string result, string? message = null)
        {
            if (string.IsNullOrEmpty(result))
            {
                throw new ArgumentNullException(nameof(result));
            }

            string content;
            using (var stream = new MemoryStream())
            using (var writer = new Utf8JsonWriter(stream))
            {
                writer.WriteStartObject();
                writer.WriteString("Result", result);
                if (!string.IsNullOrWhiteSpace(message))
                {
                    writer.WriteString("Message", message);
                }
                writer.WriteEndObject();
                writer.Flush();

                content = Encoding.UTF8.GetString(stream.ToArray());
            }
            return content;
        }
    }
}
