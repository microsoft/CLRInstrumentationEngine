// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x86", "AddNop_IfTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddMultiNop_IfTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64", "AddMultiNop_IfTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddBranchTargets_IfTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64", "AddBranchTargets_IfTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddBranchTargets_SwitchTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Release_x64", "AddBranchTargets_SwitchTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_ForTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x86", "AddNop_ForTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_WhileTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64", "AddNop_WhileTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_SwitchTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64", "AddNop_SwitchTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_SwitchTest2_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64", "AddNop_SwitchTest2_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_SwitchTest2()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Release_x64", "AddNop_SwitchTest2.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_ArrayForeachTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests2_Release_x64", "AddNop_ArrayForeachTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddBranchTargets_ArrayForeachTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests2_Release_x64", "AddBranchTargets_ArrayForeachTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void TinyMethodTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests2_Release_x64", "TinyMethodTest.xml");
        }
    }
}
