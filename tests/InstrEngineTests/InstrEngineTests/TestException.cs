// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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
    public class TestException
    {
        private static TestContext Context;

        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            Context = context;
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void ExceptionTinyMethodTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "ExceptionTests_Release_x64",
                "ExceptionTinyMethodTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void ExceptionTinyMethodTest2()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "ExceptionTests_Release_x64",
                "ExceptionTinyMethodTest2.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void ExceptionFatClauseTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "ExceptionTests_Release_x64",
                "ExceptionFatClauseTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_ExceptionMultiTryCatchTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "ExceptionTests_Release_x64",
                "RoundTrip_MultiTryCatchTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void ExceptionMultiTryCatchTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "ExceptionTests_Release_x64",
                "ExceptionMultiTryCatchTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void ExceptionFinallyTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "ExceptionTests_Release_x64",
                "ExceptionFinallyTest.xml");
        }
    }
}
