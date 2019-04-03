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
    public class TestAddInstruction
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_IfTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x86.exe", "AddNop_IfTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddMultiNop_IfTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64.exe", "AddMultiNop_IfTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_ForTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x86.exe", "AddNop_ForTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_WhileTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64.exe", "AddNop_WhileTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_SwitchTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64.exe", "AddNop_SwitchTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_SwitchTest2_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64.exe", "AddNop_SwitchTest2_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_SwitchTest2()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Release_x64.exe", "AddNop_SwitchTest2.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_ArrayForeachTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests2_Release_x64.exe", "AddNop_ArrayForeachTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void TinyMethodTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests2_Release_x64.exe", "TinyMethodTest.xml");
        }
    }
}
