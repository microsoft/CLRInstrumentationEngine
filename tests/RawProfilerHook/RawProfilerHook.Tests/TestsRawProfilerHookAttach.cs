// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RawProfilerHook.Tests
{
    using System;
    using System.Diagnostics;
    using System.IO;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using RemoteUnitTestExecutor;
    using System.Xml;

    [TestClass]
    public class TestsRawProfilerHookAttach
    {
        public class TestsRawProfilerHookIsLoadedIfEnvVariableIsSet : TestBase
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
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineProfilerModuleName, ".")]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineHostConfigName, ".")]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineDefaultMethodModuleName, ".")]
        [DeploymentItem("..\\..\\AnyCPU\\" + TestEngine.MscorlibExtensionMethodsBaseModuleName, ".")]
        [DeploymentItem(".\\" + TestEngine.RawProfilerHookModuleName, ".")]
        public void TestRawProfilerHookIsLoadedIfEnvVariableIsSet()
        {
            Assert.IsFalse(
                Process.GetCurrentProcess().IsModuleLoaded("Microsoft.RawProfilerHook"),
                "Exception extensions module is loaded");

            TestEngine.ExecuteTest<TestsRawProfilerHookIsLoadedIfEnvVariableIsSet>();
        }

        public class TestRawProfilerHookCallsGetAssemblyReferencesBase : TestBase
        {
            public override void Execute()
            {
                // load APTCA module from GAC - System.Xml
                XmlDocument doc = new XmlDocument();

                Assert.AreEqual(
                    "S_OK",
                    Environment.GetEnvironmentVariable("CCoRawProfilerHook::GetAssemblyReferences"),
                    "GetAssemblyReferences callback was not called on RawProfilerHook instance");
            }
        }

        [TestMethod]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineProfilerModuleName, ".")]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineHostConfigName, ".")]
        [DeploymentItem("..\\" + TestEngine.InstrumentationEngineDefaultMethodModuleName, ".")]
        [DeploymentItem("..\\..\\AnyCPU\\" + TestEngine.MscorlibExtensionMethodsBaseModuleName, ".")]
        [DeploymentItem(".\\" + TestEngine.RawProfilerHookModuleName, ".")]
        public void TestRawProfilerHookCallsGetAssemblyReferences()
        {
            Assert.IsFalse(
                Process.GetCurrentProcess().IsModuleLoaded("Microsoft.RawProfilerHook"),
                "Exception extensions module is loaded");

            TestEngine.ExecuteTest<TestRawProfilerHookCallsGetAssemblyReferencesBase>();
        }
    }
}
