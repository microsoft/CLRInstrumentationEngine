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
    [DeploymentItem(PathUtils.TestInstrumentationMethodX64BinPath)]
    [DeploymentItem(PathUtils.TestInstrumentationMethodX86BinPath)]
    public class TestRoundTrip
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_IfTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64.exe", "RoundTrip_IfTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_WhileTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64.exe", "RoundTrip_WhileTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_ForTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64.exe", "RoundTrip_ForTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_SwitchTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64.exe", "RoundTrip_SwitchTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_IfTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Release_x64.exe", "RoundTrip_IfTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_WhileTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Release_x64.exe", "RoundTrip_WhileTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_ForTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Release_x64.exe", "RoundTrip_ForTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_SwitchTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Release_x64.exe", "RoundTrip_SwitchTest.xml");
        }
    }
}
