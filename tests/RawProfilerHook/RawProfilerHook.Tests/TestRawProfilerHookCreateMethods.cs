// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RawProfilerHook.Tests
{
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using RemoteUnitTestExecutor;

    [TestClass]
    public class TestRawProfilerHookCreateMethods
    {
        [TestMethod]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineProfilerModuleName, ".")]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineHostConfigName, ".")]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineDefaultMethodModuleName, ".")]
        [DeploymentItem(@"..\..\AnyCPU\" + TestEngine.MscorlibExtensionMethodsBaseModuleName, ".")]
        [DeploymentItem(@".\" + TestEngine.RawProfilerHookModuleName, ".")]
        public void TestRawProfilerMethodCreate()
        {
            TestEngine.IgnoreBitness = false;
            TestEngine.ExecuteTest<TestRawProfilerMethodCreateBase>();
        }
    }

    /// <summary>
    /// WARNING! This class is subject to ICorProfiler instrumentation.
    /// Do not make this a nested class and do not modify the name.
    /// </summary>
    internal sealed class TestRawProfilerMethodCreateBase : TestBase
    {
        private int _destinationMethod = 0;

        /// <summary>
        /// This test validates three scenarios:
        /// 1) The creation and IL emit of a new method during module load (such as performed by the Desktop
        /// framework CodeCoverage profiler.
        /// 2) The creation and IL emit of a new method during JitCompilationStarted (such as performed by DataDog)
        /// 3) The creation of a new method during ModuleLoad and IL emit during JitCompilationStarted (such as
        /// profiler recommends).
        /// </summary>
        public override void Execute()
        {
            //Validate that the raw profiler emitted a new method into this type during module load.
            //Prevoiusly this would error out because we expected the method token to be cached already.

            MethodModuleLoad();
            MethodJitCompilationStarted();
            MethodModuleLoadThenJit();

            //After proper instrumentation, each stub will increment the counter.
            Assert.IsTrue(_destinationMethod == 3, "Did not reach method3. The raw profiler hook did not properly instrument the code.");
        }

        //Do not modify name, signature, or protection level without also changing the profiler.
        private void MethodModuleLoad()
        {
            /*ICorProfiler will emit:
            private void ModDefineModCreateBody()
            {
                this.DestinationMethod();
            }
            this.ModDefineModCreateBody();
            */
        }

        //Do not modify name, signature, or protection level without also changing the profiler.
        private void MethodJitCompilationStarted()
        {
            /*ICorProfiler will emit:
            class CustomTestType
            {
                public static JitDefinedJitBody(TestRawProfilerMethodCreateBase p1)
                {
                    p1.DestinationMethod();
                }
            }
            CustomTestType.JitDefinedJitBody(this);
            */
        }

        //Do not modify name, signature, or protection level without also changing the profiler.
        private void MethodModuleLoadThenJit()
        {
            /*ICorProfiler will emit:
            private void ModDefineJitCreateBody()
            {
                this.DestinationMethod();
            }
            this.ModDefineJitCreateBody();
            */
        }

        public void DestinationMethod()
        {
            _destinationMethod++;
        }
    }
}
