// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace ApplicationInsightsCompatibility
{
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using RemoteUnitTestExecutor;
    using System.Diagnostics;
    using System.IO;

    class TestEngine : TestEngineBase
    {
        private TestEngine(bool is32Bit)
            : base(is32Bit)
        {
        }

        public static ITestResult ExecuteTest<T>(bool run32Bit) where T : new()
        {
            var executor = new TestEngine(run32Bit);

            return executor.ExecuteTest(
                typeof(T).AssemblyQualifiedName,
                null);
        }

        protected override void OnBeforeStarted(Process debugee)
        {
            var vars = debugee.StartInfo.EnvironmentVariables;
            vars.Add("COR_ENABLE_PROFILING", "1");
            vars.Add("COR_PROFILER", "{324F817A-7420-4E6D-B3C1-143FBED6D855}");
            vars.Add("COR_PROFILER_PATH", GetFullPath(TestFiles.GetInstrumentationEngineProfilerModuleName(Is32Bit)));

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
    }
}
