// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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
    public class TestDynamicCode
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void CompiledDynamicCodeTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("DynamicCodeTests_Debug_x64.exe", "DynamicCompiledCode.xml", "Compiled");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void EmittedDynamicCodeTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("DynamicCodeTests_Debug_x64.exe", "DynamicEmittedCode.xml", "Emitted");
        }
    }
}
