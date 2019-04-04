// Copyright (c) Microsoft Corporation. All rights reserved.
// 

namespace ApplicationInsightsCompatibility
{
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using RemoteUnitTestExecutor;
    using System.IO;

    class TestEngine : TestEngineBase
    {
#if X64
        public const string InstrumentationEngineProfilerModuleName = "MicrosoftInstrumentationEngine_x64.dll";
        public const string InstrumentationEngineDefaultMethodModuleName = "Microsoft.InstrumentationEngine.Extensions.Base_x64.dll";
        private const bool IsX86 = false;
        public const string RawProfilerHookModuleName = "Microsoft.RawProfilerHook_x64.dll";
#else
        public const string InstrumentationEngineProfilerModuleName = "MicrosoftInstrumentationEngine_x86.dll";
        public const string InstrumentationEngineDefaultMethodModuleName = "Microsoft.InstrumentationEngine.Extensions.Base_x86.dll";
        private const bool IsX86 = true;
        public const string RawProfilerHookModuleName = "Microsoft.RawProfilerHook_x86.dll";
#endif

        public const string MscorlibExtensionMethodsBaseModuleName = "Microsoft.Diagnostics.Instrumentation.Extensions.Base.dll";
        public const string InstrumentationEngineHostConfigName = "Microsoft.InstrumentationEngine.Extensions.config";
        private const string InstrumentationEngineProfilerId = "{324F817A-7420-4E6D-B3C1-143FBED6D855}";
        private const string InstrumentationEngineApmcExtensionApmcId = "{CA487940-57D2-10BF-11B2-A3AD5A13CBC0}";
        private const string RawProfilerHookComponentId = "{4BCBE156-37EC-4179-B051-0183C57BACF9}";

        private TestEngine()
        {
        }

        public static ITestResult ExecuteTest<T>()
        {
            var executor = new TestEngine();

            return executor.ExecuteTest(
                typeof(T).AssemblyQualifiedName,
                IsX86,
                null);
        }

        protected override void OnBeforeStarted(DebugeeProcess debugee)
        {
            var vars = debugee.StartInfo.EnvironmentVariables;
            vars.Add("COR_ENABLE_PROFILING", "1");
            vars.Add("COR_PROFILER", "{324F817A-7420-4E6D-B3C1-143FBED6D855}");
            vars.Add("COR_PROFILER_PATH", GetFullPath(InstrumentationEngineProfilerModuleName));

            vars.Add("MicrosoftInstrumentationEngine_FileLog",  "Dumps|Errors");
            //vars.Add("MicrosoftInstrumentationEngine_FileLogPath", ""); //can be overridden if needed

#if ALLOWNOTSIGNED
            vars.Add("MicrosoftInstrumentationEngine_DisableCodeSignatureValidation", "true");
#endif
        }

        private string GetFullPath(string moduleName)
        {
            var result = Path.Combine(Directory.GetCurrentDirectory(), moduleName);
            Assert.IsTrue(File.Exists(result), $"File {result} doesn't exists");
            return result;
        }

        protected override void OnStarted(DebugeeProcess debugee)
        {
        }

        protected override void OnComplete(DebugeeProcess debugee, string[] expectedErrors = null)
        {
        }
    }
}
