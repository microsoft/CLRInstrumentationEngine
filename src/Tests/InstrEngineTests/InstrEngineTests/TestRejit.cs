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
    public class TestRejit
    {
        private static TestContext Context;

        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            Context = context;
        }

        /// <summary>
        /// Use existing test script and test baseline for rejit test
        /// </summary>

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_RoundTrip_IfTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(
                TestParameters.FromContext(Context),
                testApp: "BasicManagedTests_Release_x64",
                testScript: "RoundTrip_IfTest.xml",
                output: "Rejit_RoundTrip_IfTest.xml");

            ProfilerHelpers.DiffResultToBaseline(Context, output: "Rejit_RoundTrip_IfTest.xml", baseline: "RoundTrip_IfTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_RoundTrip_SwitchTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(
                TestParameters.FromContext(Context),
                testApp: "BasicManagedTests_Release_x64",
                testScript: "RoundTrip_SwitchTest.xml",
                output: "Rejit_RoundTrip_SwitchTest.xml");

            ProfilerHelpers.DiffResultToBaseline(Context, output: "Rejit_RoundTrip_SwitchTest.xml", baseline: "RoundTrip_SwitchTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_RoundTrip_ForTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(
                TestParameters.FromContext(Context),
                testApp: "BasicManagedTests_Release_x64",
                testScript: "RoundTrip_ForTest.xml",
                output: "Rejit_RoundTrip_ForTest.xml");

            ProfilerHelpers.DiffResultToBaseline(Context, output: "Rejit_RoundTrip_ForTest.xml", baseline: "RoundTrip_ForTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_AddNop_SwitchTest2()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(
                TestParameters.FromContext(Context),
                testApp: "BasicManagedTests_Release_x64",
                testScript: "AddNop_SwitchTest2.xml",
                output: "Rejit_AddNop_SwitchTest2.xml");

            ProfilerHelpers.DiffResultToBaseline(Context, output: "Rejit_AddNop_SwitchTest2.xml", baseline: "AddNop_SwitchTest2.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_ExceptionFinallyTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(
                TestParameters.FromContext(Context),
                testApp: "ExceptionTests_Release_x64",
                testScript: "ExceptionFinallyTest.xml",
                output: "Rejit_ExceptionFinallyTest.xml");

            ProfilerHelpers.DiffResultToBaseline(Context, output: "Rejit_ExceptionFinallyTest.xml", baseline: "ExceptionFinallyTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_ExceptionMultiTryCatchTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(
                TestParameters.FromContext(Context),
                testApp: "ExceptionTests_Release_x64",
                testScript: "ExceptionMultiTryCatchTest.xml",
                output: "Rejit_ExceptionMultiTryCatchTest.xml");

            ProfilerHelpers.DiffResultToBaseline(Context, output: "Rejit_ExceptionMultiTryCatchTest.xml", baseline: "ExceptionMultiTryCatchTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_Instru_RemoveAllTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(
                TestParameters.FromContext(Context),
                testApp: "InstruOperationsTests_Release_x64",
                testScript: "Instru_RemoveAllTest.xml",
                output: "Rejit_Instru_RemoveAllTest.xml",
                args: "Instru_CreateErrorTest");

            ProfilerHelpers.DiffResultToBaseline(Context, output: "Rejit_Instru_RemoveAllTest.xml", baseline: "Instru_RemoveAllTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Rejit_Instru_ReplaceTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(
                TestParameters.FromContext(Context),
                testApp: "InstruOperationsTests_Release_x64",
                testScript: "Instru_ReplaceTest.xml",
                output: "Rejit_Instru_ReplaceTest.xml",
                args: "Instru_ReplaceTest");

            ProfilerHelpers.DiffResultToBaseline(Context, output: "Rejit_Instru_ReplaceTest.xml", baseline: "Instru_ReplaceTest.xml");
        }
    }
}
