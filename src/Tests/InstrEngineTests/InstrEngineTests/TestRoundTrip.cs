// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace InstrEngineTests
{
    [TestClass]
    // "dotnet test" runs tests in-place (instead of copying them to a deployment
    // directory). DeploymentItems that copy from a source path to a destination
    // that is the same location are not necessary and cause file locking issues
    // when initializing the tests. Only use DeploymentItem for paths with different
    // destinations for .NET Core test assemblies.
#if !NETCOREAPP
    [DeploymentItem(PathUtils.BaselinesBinPath, PathUtils.BaselinesBinPath)]
    [DeploymentItem(PathUtils.TestScriptsBinPath, PathUtils.TestScriptsBinPath)]
#endif
    [DeploymentItem(PathUtils.InstrumentationEngineX64BinPath)]
    [DeploymentItem(PathUtils.InstrumentationEngineX86BinPath)]
    [DeploymentItem(PathUtils.NaglerInstrumentationConfigX64BinPath)]
    [DeploymentItem(PathUtils.NaglerInstrumentationConfigX86BinPath)]
    [DeploymentItem(PathUtils.NaglerInstrumentationMethodX64BinPath)]
    [DeploymentItem(PathUtils.NaglerInstrumentationMethodX86BinPath)]
    [DeploymentItem(PathUtils.LinuxInstrumentationEngineX64BinPath)]
    [DeploymentItem(PathUtils.LinuxNaglerInstrumentationConfigX64BinPath)]
    [DeploymentItem(PathUtils.LinuxNaglerInstrumentationMethodX64BinPath)]
    public class TestRoundTrip
    {
        private static TestContext Context;

        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            Context = context;
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_IfTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Debug_x64",
                "RoundTrip_IfTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_WhileTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Debug_x64",
                "RoundTrip_WhileTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_ForTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Debug_x64",
                "RoundTrip_ForTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_SwitchTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Debug_x64",
                "RoundTrip_SwitchTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_IfTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Release_x64",
                "RoundTrip_IfTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_WhileTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Release_x64",
                "RoundTrip_WhileTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_ForTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Release_x64",
                "RoundTrip_ForTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RoundTrip_SwitchTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Release_x64",
                "RoundTrip_SwitchTest.xml");
        }
    }
}
