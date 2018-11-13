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
    public class TestInjectToMscorlib
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void SimpleInjectTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("InjectToMscorlibTest_Release_x64", "InjectToMscorlibTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void SimpleInjectTest32()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("InjectToMscorlibTest_Release_x86", "InjectToMscorlibTest32.xml");
        }
    }
}
