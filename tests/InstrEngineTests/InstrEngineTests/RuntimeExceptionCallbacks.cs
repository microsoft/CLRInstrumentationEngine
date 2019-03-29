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
    [DeploymentItem(PathUtils.TestInstrumentationMethodX64BinPath)]
    [DeploymentItem(PathUtils.TestInstrumentationMethodX86BinPath)]
    public class RuntimeExceptionCallbacks
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void RuntimeExceptionCallbacksTest()
        {
            ProfilerHelpers.LaunchAppUnderProfiler(testApp: "RuntimeExceptionCallbacks_Debug_x64.exe", testScript: "RuntimeExceptionCallbacks.xml",
                output: "RuntimeExceptionCallbacks.xml", isRejit: false, args: null);

            ProfilerHelpers.DiffResultToBaseline(output: "RuntimeExceptionCallbacks.xml", baseline: "RuntimeExceptionCallbacks.xml");
        }
    }
}
