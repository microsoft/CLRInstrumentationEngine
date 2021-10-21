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
    public class MaxStackSizeTests
    {
        private static TestContext Context;

        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            Context = context;
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void TestMaxStackSize_WhenBranchingMatters()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "MaxStackSizeExample_Release_x64",
                "Empty_CalculateMaxStackSizeTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void TestMaxStackSize_WhenMethodWithExceptionFilter()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "MaxStackSizeExample_Release_x64",
                "Empty_CalculateMaxStackSizeExceptionFilterTest.xml");
        }
    }
}
