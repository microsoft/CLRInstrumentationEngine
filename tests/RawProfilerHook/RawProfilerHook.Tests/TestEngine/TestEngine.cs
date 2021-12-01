// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RawProfilerHook.Tests
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.IO;
    using System.Text;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using RemoteUnitTestExecutor;
    using System.Threading;

    /// <summary>
    ///     Summary description for TestsExtensionHostLoading
    /// </summary>
    public class TestEngine : RemoteUnitTestExecutor.TestEngineBase
    {
        public static bool IgnoreBitness { get; set; }
#if X64
        public const string InstrumentationEngineProfilerModuleName = "MicrosoftInstrumentationEngine_x64.dll";
        public const string InstrumentationEngineDefaultMethodModuleName = "Microsoft.InstrumentationEngine.Extensions.Base_x64.dll";
        public const string InstrumentationEngineEnvVar = "COR_PROFILER_PATH_64";
        private const bool IsX86 = false;
        public const string RawProfilerHookPathEnvVar = "MicrosoftInstrumentationEngine_RawProfilerHookPath_64";
        public const string RawProfilerHookModuleName = "Microsoft.RawProfilerHook_x64.dll";
#else
        public const string InstrumentationEngineProfilerModuleName = "MicrosoftInstrumentationEngine_x86.dll";
        public const string InstrumentationEngineDefaultMethodModuleName = "Microsoft.InstrumentationEngine.Extensions.Base_x86.dll";
        public const string InstrumentationEngineEnvVar = "COR_PROFILER_PATH_32";
        private const bool IsX86 = true;
        public const string RawProfilerHookPathEnvVar = "MicrosoftInstrumentationEngine_RawProfilerHookPath_32";
        public const string RawProfilerHookModuleName = "Microsoft.RawProfilerHook_x86.dll";
#endif

        public const string MscorlibExtensionMethodsBaseModuleName = "Microsoft.Diagnostics.Instrumentation.Extensions.Base.dll";

        public const string InstrumentationEngineNoBitnessEnvVar = "COR_PROFILER_PATH";
        public const string InstrumentationEngineHostConfigName = "Microsoft.InstrumentationEngine.Extensions.config";
        private const string InstrumentationEngineProfilerId = "{324F817A-7420-4E6D-B3C1-143FBED6D855}";

        public const string RawProfilerHookEnvVar = "MicrosoftInstrumentationEngine_RawProfilerHook";
        public const string RawProfilerHookPathNoBitnessEnvVar = "MicrosoftInstrumentationEngine_RawProfilerHookPath";
        private const string RawProfilerHookComponentId = "{4BCBE156-37EC-4179-B051-0183C57BACF9}";

        public const string ProfilerSrcFolder = @".\";

        private readonly string traceFilePath;

        public static ITestResult ExecuteTest<T>() where T : new()
        {
            var executor = new TestEngine();

            return executor.ExecuteTest(
                typeof(T).AssemblyQualifiedName,
                IsX86,
                null);
        }

        public TestEngine()
        {
            this.traceFilePath = Path.GetFullPath(Path.Combine(Directory.GetCurrentDirectory(), $"profiler_trace_{Guid.NewGuid().ToString()}.txt"));
        }

        public ITestResult ExecuteTest<T>(string[] expectedErrors = null)
        {
            return ExecuteTest(typeof(T).AssemblyQualifiedName, IsX86, expectedErrors);
        }

        public ITestResult ExecuteTest(Type t, string[] expectedErrors = null)
        {
            if (null == t)
            {
                throw new ArgumentNullException(nameof(t));
            }

            return ExecuteTest(t.AssemblyQualifiedName, IsX86, expectedErrors);
        }

        protected override void OnBeforeStarted(Process debugee)
        {
            if (debugee == null)
            {
                throw new ArgumentNullException(nameof(debugee));
            }

            string profilerDest = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(typeof(TestEngine).Assembly.Location), ProfilerSrcFolder));

            Trace.TraceInformation("profilerDest: {0}", profilerDest);

            var hostEnvironment = new Dictionary<string, string>
            {
                // { "MicrosoftInstrumentationEngine_DebugWait", "1" }, // Attach native debugger to RemoteUnitTestExecutor.Host_x86|64.exe which spawns once this function finishes
                { "COR_ENABLE_PROFILING", "1" },
                { "COR_PROFILER", InstrumentationEngineProfilerId },
                { RawProfilerHookEnvVar, RawProfilerHookComponentId },
                { "MicrosoftInstrumentationEngine_FileLog", "Dumps|Errors" },
                { "MicrosoftInstrumentationEngine_FileLogPath", traceFilePath },
                { "MicrosoftInstrumentationEngine_UserBuffer", "1" }
#if ALLOWNOTSIGNED
                , { "MicrosoftInstrumentationEngine_DisableCodeSignatureValidation", "true"}
#endif
            };

            if (IgnoreBitness)
            {
                hostEnvironment.Add(
                    InstrumentationEngineNoBitnessEnvVar,
                    GetFullPath(InstrumentationEngineProfilerModuleName));

                hostEnvironment.Add(
                    RawProfilerHookPathNoBitnessEnvVar,
                    GetFullPath(RawProfilerHookModuleName));
            }
            else
            {
                hostEnvironment.Add(
                    InstrumentationEngineEnvVar,
                    GetFullPath(InstrumentationEngineProfilerModuleName));

                hostEnvironment.Add(
                    RawProfilerHookPathEnvVar,
                    GetFullPath(RawProfilerHookModuleName));
            }

            foreach (var variable in hostEnvironment)
            {
                debugee.StartInfo.EnvironmentVariables.Add(variable.Key, variable.Value);
            }

        }

        private string ValidatePath(string path)
        {
            Assert.IsTrue(File.Exists(path), $"File {path} doesn't exists");
            return path;
        }

        private string GetFullPath(string moduleName)
        {
            var result = Path.Combine(Directory.GetCurrentDirectory(), moduleName);
            ValidatePath(result);
            return result;
        }

        protected override void OnComplete(Process debugee, string[] expectedErrors = null)
        {
            string errors = string.Empty;
            try
            {
                if (File.Exists(traceFilePath))
                {
                    bool readOk = false;
                    int attemptsCount = 3;
                    while (attemptsCount > 0 && !readOk)
                    {
                        --attemptsCount;
                        try
                        {
                            using (var traceFile = new StreamReader(traceFilePath))
                            {
                                while (!traceFile.EndOfStream)
                                {
                                    string traceLine = traceFile.ReadLine();
                                    if (traceLine.StartsWith("LogError", StringComparison.Ordinal))
                                    {
                                        errors += traceLine + "\r\n";
                                    }
                                    if (traceLine.Length != 0 &&
                                        !traceLine.StartsWith("[TestIgnore]", StringComparison.Ordinal))
                                    {
                                        Trace.WriteLine(traceLine);
                                    }
                                }
                                readOk = true;
                            }
                        }
                        catch (Exception)
                        {
                            //retry
                            Thread.Sleep(1000);
                        }

                    }
                }

            }
            catch (Exception)
            {
                //Do nothing
            }

            if (!string.IsNullOrEmpty(errors))
            {
                if (expectedErrors != null)
                {
                    var actualErrors = errors.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);

                    var testFailureDescription = new StringBuilder();

                    Assert.AreEqual(expectedErrors.Length, actualErrors.Length, "unexpected errors: " + errors);
                    for (int i = 0; i < actualErrors.Length; i++)
                    {
                        if (!actualErrors[i].Contains(expectedErrors[i]))
                        {
                            testFailureDescription.AppendLine("Expected to contain :");
                            testFailureDescription.AppendLine(expectedErrors[i]);

                            testFailureDescription.AppendLine("Actual content:");
                            testFailureDescription.AppendLine(actualErrors[i]);
                        }
                    }

                    if (0 != testFailureDescription.Length)
                    {
                        Assert.Fail(
                            "Unexpected errors detected:" + Environment.NewLine + testFailureDescription);
                    }
                }
                else
                {
                    Assert.Fail("Unexpected errors detected: " + errors);
                }
            }
        }
    }
}
