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
    public class TestInstrumentationMethodLogging
    {
        private static TestContext Context;

        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            Context = context;
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void InstrMethodLoggingTest()
        {
            TestParameters parameters = TestParameters.FromContext(Context);
            parameters.DisableMethodPrefix = false;
            ProfilerHelpers.LaunchAppAndCompareResult(
                parameters,
                testApp: "InstrumentationMethodLoggingTests_Debug_x64",
                fileName: "InstrumentationMethodLogging.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void InstrMethodLoggingTestLevelNone()
        {
            TestParameters parameters = TestParameters.FromContext(Context);
            parameters.DisableMethodPrefix = false;
            parameters.MethodLogLevel = LogLevel.None;
            ProfilerHelpers.LaunchAppAndCompareResult(
                parameters,
                testApp: "InstrumentationMethodLoggingTests_Debug_x64",
                fileName: "InstrumentationMethodLoggingNone.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void InstrMethodLoggingTestLevelAll()
        {
            TestParameters parameters = TestParameters.FromContext(Context);
            parameters.DisableMethodPrefix = false;
            parameters.MethodLogLevel = LogLevel.All;
            ProfilerHelpers.LaunchAppAndCompareResult(
                parameters,
                testApp: "InstrumentationMethodLoggingTests_Debug_x64",
                fileName: "InstrumentationMethodLoggingAll.xml",
                regexCompare: true);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void InstrMethodLoggingTestLevelSome()
        {
            TestParameters parameters = TestParameters.FromContext(Context);
            parameters.DisableMethodPrefix = false;
            parameters.MethodLogLevel = LogLevel.Errors | LogLevel.InstrumentationResults;
            ProfilerHelpers.LaunchAppAndCompareResult(
                parameters,
                testApp: "InstrumentationMethodLoggingTests_Debug_x64",
                fileName: "InstrumentationMethodLoggingSome.xml",
                regexCompare: true);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void InstrMethodLoggingTestLevelAllWithNoLogLevel()
        {
            TestParameters parameters = TestParameters.FromContext(Context);
            parameters.DisableMethodPrefix = false;
            parameters.DisableLogLevel = true;
            parameters.MethodLogLevel = LogLevel.All;
            ProfilerHelpers.LaunchAppAndCompareResult(
                parameters,
                testApp: "InstrumentationMethodLoggingTests_Debug_x64",
                fileName: "InstrumentationMethodLoggingAllWithNoLogLevel.xml",
                regexCompare: true);
        }
    }
}
