// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RawProfilerHook.Tests
{
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using RemoteUnitTestExecutor;
    using System;
    using System.Diagnostics;
    using System.Xml;

    [TestClass]
    public class TestsRawProfilerHookAttach
    {
        private class TestsRawProfilerHookIsLoadedIfEnvVariableIsSet : TestBase
        {
            public override void Execute()
            {
                Assert.IsTrue(
                    Process.GetCurrentProcess().IsModuleLoaded("Microsoft.RawProfilerHook"),
                    "Raw profiler module is not loaded correctly");

                Assert.AreEqual(
                    "S_OK",
                    Environment.GetEnvironmentVariable("CCoRawProfilerHook::Initialize"),
                    "Initialize callback was not called on RawProfilerHook instance");
            }
        }

        [TestMethod]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineProfilerModuleName32, ".")]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineHostConfigName, ".")]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineDefaultMethodModuleName32, ".")]
        [DeploymentItem(@"..\..\AnyCPU\" + TestEngine.MscorlibExtensionMethodsBaseModuleName, ".")]
        [DeploymentItem(@".\" + TestEngine.RawProfilerHookModuleName32, ".")]
        public void TestRawProfilerHookIsLoadedIfEnvVariableIsSet32()
        {
            Assert.IsFalse(
                Process.GetCurrentProcess().IsModuleLoaded("Microsoft.RawProfilerHook"),
                "Exception extensions module is loaded");

            TestEngine.ExecuteTest<TestsRawProfilerHookIsLoadedIfEnvVariableIsSet>(run32Bit: true, ignoreBitness: true);

            TestEngine.ExecuteTest<TestsRawProfilerHookIsLoadedIfEnvVariableIsSet>(run32Bit: true, ignoreBitness: false);
        }

        [TestMethod]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineProfilerModuleName64, ".")]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineHostConfigName, ".")]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineDefaultMethodModuleName64, ".")]
        [DeploymentItem(@"..\..\AnyCPU\" + TestEngine.MscorlibExtensionMethodsBaseModuleName, ".")]
        [DeploymentItem(@".\" + TestEngine.RawProfilerHookModuleName64, ".")]
        public void TestRawProfilerHookIsLoadedIfEnvVariableIsSet64()
        {
            Assert.IsFalse(
                Process.GetCurrentProcess().IsModuleLoaded("Microsoft.RawProfilerHook"),
                "Exception extensions module is loaded");

            TestEngine.ExecuteTest<TestsRawProfilerHookIsLoadedIfEnvVariableIsSet>(run32Bit: false, ignoreBitness: true);

            TestEngine.ExecuteTest<TestsRawProfilerHookIsLoadedIfEnvVariableIsSet>(run32Bit: false, ignoreBitness: false);
        }

        private class TestRawProfilerHookCallsGetAssemblyReferencesBase : TestBase
        {
            public override void Execute()
            {
                // load APTCA module from GAC - System.Xml
                XmlDocument doc = new XmlDocument() { XmlResolver = null };

                Assert.AreEqual(
                    "S_OK",
                    Environment.GetEnvironmentVariable("CCoRawProfilerHook::GetAssemblyReferences"),
                    "GetAssemblyReferences callback was not called on RawProfilerHook instance");
            }
        }

        // BUG: CLRIE does not receive ICorProfilerCallback6::GetAssemblyReferences callback.
        // See https://github.com/microsoft/CLRInstrumentationEngine/issues/414 for details.
        [TestMethod]
        [Ignore("https://github.com/microsoft/CLRInstrumentationEngine/issues/414")]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineProfilerModuleName32, ".")]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineHostConfigName, ".")]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineDefaultMethodModuleName32, ".")]
        [DeploymentItem(@"..\..\AnyCPU\" + TestEngine.MscorlibExtensionMethodsBaseModuleName, ".")]
        [DeploymentItem(@".\" + TestEngine.RawProfilerHookModuleName32, ".")]
        public void TestRawProfilerHookCallsGetAssemblyReferences32()
        {
            Assert.IsFalse(
                Process.GetCurrentProcess().IsModuleLoaded("Microsoft.RawProfilerHook"),
                "Exception extensions module is loaded");

            TestEngine.ExecuteTest<TestRawProfilerHookCallsGetAssemblyReferencesBase>(run32Bit: true, ignoreBitness: true);

            TestEngine.ExecuteTest<TestRawProfilerHookCallsGetAssemblyReferencesBase>(run32Bit: true, ignoreBitness: false);
        }

        // BUG: CLRIE does not receive ICorProfilerCallback6::GetAssemblyReferences callback.
        // See https://github.com/microsoft/CLRInstrumentationEngine/issues/414 for details.
        [TestMethod]
        [Ignore("https://github.com/microsoft/CLRInstrumentationEngine/issues/414")]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineProfilerModuleName64, ".")]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineHostConfigName, ".")]
        [DeploymentItem(@"..\" + TestEngine.InstrumentationEngineDefaultMethodModuleName64, ".")]
        [DeploymentItem(@"..\..\AnyCPU\" + TestEngine.MscorlibExtensionMethodsBaseModuleName, ".")]
        [DeploymentItem(@".\" + TestEngine.RawProfilerHookModuleName64, ".")]
        public void TestRawProfilerHookCallsGetAssemblyReferences64()
        {
            Assert.IsFalse(
                Process.GetCurrentProcess().IsModuleLoaded("Microsoft.RawProfilerHook"),
                "Exception extensions module is loaded");

            TestEngine.ExecuteTest<TestRawProfilerHookCallsGetAssemblyReferencesBase>(run32Bit: false, ignoreBitness: true);

            TestEngine.ExecuteTest<TestRawProfilerHookCallsGetAssemblyReferencesBase>(run32Bit: false, ignoreBitness: false);
        }
    }
}
