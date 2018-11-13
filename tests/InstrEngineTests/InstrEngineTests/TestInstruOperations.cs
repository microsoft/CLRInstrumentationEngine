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
    [DeploymentItem(PathUtils.NaglerProfilerHostX64BinPath)]
    [DeploymentItem(PathUtils.NaglerProfilerHostX86BinPath)]
    public class TestInstruOperations
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_RemoveAllTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("InstruOperationsTests_Release_x64", "Instru_RemoveAllTest.xml", "Instru_CreateErrorTest");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_EmptyMethodRemoveAllTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("InstruOperationsTests_Release_x64", "Instru_EmptyMethodRemoveAllTest.xml", "Instru_EmptyMethodTest");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_ReplaceTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("InstruOperationsTests_Release_x64", "Instru_ReplaceTest.xml", "Instru_ReplaceTest");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_ReplaceTest2()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("InstruOperationsTests_Release_x64", "Instru_ReplaceTest2.xml", "Instru_CreateErrorTest");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_RemoveTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("InstruOperationsTests_Release_x64", "Instru_RemoveTest.xml", "Instru_RemoveTest");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_RemoveTest2()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("InstruOperationsTests_Release_x64", "Instru_RemoveTest2.xml", "Instru_CreateErrorTest");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_InsterAfterTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("InstruOperationsTests_Release_x64", "Instru_InsertAfterTest.xml", "Instru_InsertAfterTest");
        }
    }
}
