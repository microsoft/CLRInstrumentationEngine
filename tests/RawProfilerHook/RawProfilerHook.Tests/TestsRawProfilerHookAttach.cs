// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RawProfilerHook.Tests
{
    using Intercept.Tests;
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
        [DeploymentItem(TestFiles.DeploymentItem_InstrumentationEngineModule32, TestFiles.DeploymentTarget_Instrumentation32)]
        [DeploymentItem(TestFiles.DeploymentItem_ManagedExtensionsBaseModule, TestFiles.DeploymentTarget_Instrumentation32)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseConfig32, TestFiles.DeploymentTarget_Instrumentation32)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseModule32, TestFiles.DeploymentTarget_Instrumentation32)]
        [DeploymentItem(TestFiles.DeploymentItem_RawProfilerHookModule32, TestFiles.DeploymentTarget_Instrumentation32)]
        public void TestRawProfilerHookIsLoadedIfEnvVariableIsSet32()
        {
            Assert.IsFalse(
                Process.GetCurrentProcess().IsModuleLoaded("Microsoft.RawProfilerHook"),
                "Exception extensions module is loaded");

            TestEngine.ExecuteTest<TestsRawProfilerHookIsLoadedIfEnvVariableIsSet>(run32Bit: true, ignoreBitness: true);

            TestEngine.ExecuteTest<TestsRawProfilerHookIsLoadedIfEnvVariableIsSet>(run32Bit: true, ignoreBitness: false);
        }

        [TestMethod]
        [DeploymentItem(TestFiles.DeploymentItem_InstrumentationEngineModule64, TestFiles.DeploymentTarget_Instrumentation64)]
        [DeploymentItem(TestFiles.DeploymentItem_ManagedExtensionsBaseModule, TestFiles.DeploymentTarget_Instrumentation64)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseConfig64, TestFiles.DeploymentTarget_Instrumentation64)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseModule64, TestFiles.DeploymentTarget_Instrumentation64)]
        [DeploymentItem(TestFiles.DeploymentItem_RawProfilerHookModule64, TestFiles.DeploymentTarget_Instrumentation64)]
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
        [DeploymentItem(TestFiles.DeploymentItem_InstrumentationEngineModule32, TestFiles.DeploymentTarget_Instrumentation32)]
        [DeploymentItem(TestFiles.DeploymentItem_ManagedExtensionsBaseModule, TestFiles.DeploymentTarget_Instrumentation32)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseConfig32, TestFiles.DeploymentTarget_Instrumentation32)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseModule32, TestFiles.DeploymentTarget_Instrumentation32)]
        [DeploymentItem(TestFiles.DeploymentItem_RawProfilerHookModule32, TestFiles.DeploymentTarget_Instrumentation32)]
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
        [DeploymentItem(TestFiles.DeploymentItem_InstrumentationEngineModule64, TestFiles.DeploymentTarget_Instrumentation64)]
        [DeploymentItem(TestFiles.DeploymentItem_ManagedExtensionsBaseModule, TestFiles.DeploymentTarget_Instrumentation64)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseConfig64, TestFiles.DeploymentTarget_Instrumentation64)]
        [DeploymentItem(TestFiles.DeploymentItem_NativeExtensionsBaseModule64, TestFiles.DeploymentTarget_Instrumentation64)]
        [DeploymentItem(TestFiles.DeploymentItem_RawProfilerHookModule64, TestFiles.DeploymentTarget_Instrumentation64)]
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
