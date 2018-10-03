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
    [DeploymentItem(PathUtils.NaglerProfilerHostX64BinPath)]
    [DeploymentItem(PathUtils.NaglerProfilerHostX86BinPath)]
    public class TestLocals
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        [TestMethod]
        [Timeout(60000)]
        public void Instru_Locals()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("LocalTests_Release_x64", "Instru_Locals.xml", "AddLocals");
        }

        [TestMethod]
        [Timeout(60000)]
        public void Instru_LocalsExisting()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("LocalTests_Debug_x64", "Instru_LocalsExisting.xml", "AddLocalsExisting");
        }
    }
}
