// Copyright (c) Microsoft Corporation. All rights reserved.
//

using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace InstrEngineTests
{
    [TestClass]
    [DeploymentItem(PathUtils.BaselinesBinPath, PathUtils.BaselinesBinPath)]
    [DeploymentItem(PathUtils.TestScriptsBinPath, PathUtils.TestScriptsBinPath)]
    [DeploymentItem(PathUtils.InstrumentationEngineX64BinPath)]
    [DeploymentItem(PathUtils.InstrumentationEngineX86BinPath)]
    [DeploymentItem(PathUtils.InstrumentationConfigX64BinPath)]
    [DeploymentItem(PathUtils.InstrumentationConfigX86BinPath)]
    [DeploymentItem(PathUtils.NaglerInstrumentationMethodX64BinPath)]
    [DeploymentItem(PathUtils.NaglerInstrumentationMethodX86BinPath)]
    public class TestRejit
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        /// <summary>
        /// Use existing test script and test baseline for rejit test
        /// </summary>

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_RoundTrip_IfTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(testApp: "BasicManagedTests_Release_x64.exe", testScript: "RoundTrip_IfTest.xml",
                output: "Rejit_RoundTrip_IfTest.xml", isRejit: true, args: null);

            ProfilerHelpers.DiffResultToBaseline(output: "Rejit_RoundTrip_IfTest.xml", baseline: "RoundTrip_IfTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_RoundTrip_SwitchTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(testApp: "BasicManagedTests_Release_x64.exe", testScript: "RoundTrip_SwitchTest.xml",
                output: "Rejit_RoundTrip_SwitchTest.xml", isRejit: true, args: null);

            ProfilerHelpers.DiffResultToBaseline(output: "Rejit_RoundTrip_SwitchTest.xml", baseline: "RoundTrip_SwitchTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_RoundTrip_ForTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(testApp: "BasicManagedTests_Release_x64.exe", testScript: "RoundTrip_ForTest.xml",
                output: "Rejit_RoundTrip_ForTest.xml", isRejit: true, args: null);

            ProfilerHelpers.DiffResultToBaseline(output: "Rejit_RoundTrip_ForTest.xml", baseline: "RoundTrip_ForTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_AddNop_SwitchTest2()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(testApp: "BasicManagedTests_Release_x64.exe", testScript: "AddNop_SwitchTest2.xml",
                output: "Rejit_AddNop_SwitchTest2.xml", isRejit: true, args: null);

            ProfilerHelpers.DiffResultToBaseline(output: "Rejit_AddNop_SwitchTest2.xml", baseline: "AddNop_SwitchTest2.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_ExceptionFinallyTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(testApp: "ExceptionTests_Release_x64.exe", testScript: "ExceptionFinallyTest.xml",
                output: "Rejit_ExceptionFinallyTest.xml", isRejit: true, args: null);

            ProfilerHelpers.DiffResultToBaseline(output: "Rejit_ExceptionFinallyTest.xml", baseline: "ExceptionFinallyTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_ExceptionMultiTryCatchTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(testApp: "ExceptionTests_Release_x64.exe", testScript: "ExceptionMultiTryCatchTest.xml",
                output: "Rejit_ExceptionMultiTryCatchTest.xml", isRejit: true, args: null);

            ProfilerHelpers.DiffResultToBaseline(output: "Rejit_ExceptionMultiTryCatchTest.xml", baseline: "ExceptionMultiTryCatchTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_Instru_RemoveAllTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(testApp: "InstruOperationsTests_Release_x64.exe", testScript: "Instru_RemoveAllTest.xml",
                output: "Rejit_Instru_RemoveAllTest.xml", isRejit: true, args: "Instru_CreateErrorTest");

            ProfilerHelpers.DiffResultToBaseline(output: "Rejit_Instru_RemoveAllTest.xml", baseline: "Instru_RemoveAllTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_Instru_ReplaceTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(testApp: "InstruOperationsTests_Release_x64.exe", testScript: "Instru_ReplaceTest.xml",
                output: "Rejit_Instru_ReplaceTest.xml", isRejit: true, args: "Instru_ReplaceTest");

            ProfilerHelpers.DiffResultToBaseline(output: "Rejit_Instru_ReplaceTest.xml", baseline: "Instru_ReplaceTest.xml");
        }
    }
}
