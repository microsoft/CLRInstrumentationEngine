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
    [DeploymentItem(PathUtils.NaglerProfilerHostX64BinPath)]
    [DeploymentItem(PathUtils.NaglerProfilerHostX86BinPath)]
    public class TestHttpMethods
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        // Disabling test because HttpMethodTests_Debug.xml test script does not exist
        //[TestMethod]
        [Timeout(60000)]
        public void GetResponseAsync()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("HttpMethodTests_Debug.exe", "HttpMethodTests_Debug.xml");
        }
    }
}
