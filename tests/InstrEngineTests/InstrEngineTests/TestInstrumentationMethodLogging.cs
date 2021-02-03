﻿// Copyright (c) Microsoft Corporation. All rights reserved.
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
    [DeploymentItem(PathUtils.NaglerInstrumentationMethodX64BinPath)]
    [DeploymentItem(PathUtils.NaglerInstrumentationMethodX86BinPath)]
    public class TestInstrumentationMethodLogging
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void InstrMethodLoggingTest()
        {
            TestParameters.DisableMethodPrefix = false;
            ProfilerHelpers.LaunchAppAndCompareResult(testApp: "InstrumentationMethodLoggingTests_Debug_x64", fileName: "InstrumentationMethodLogging.xml", args: null);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void InstrMethodLoggingTestLevelNone()
        {
            TestParameters.DisableMethodPrefix = false;
            TestParameters.MethodLogLevel = LogLevel.None;
            ProfilerHelpers.LaunchAppAndCompareResult(testApp: "InstrumentationMethodLoggingTests_Debug_x64", fileName: "InstrumentationMethodLoggingNone.xml", args: null);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void InstrMethodLoggingTestLevelAll()
        {
            TestParameters.DisableMethodPrefix = false;
            TestParameters.MethodLogLevel = LogLevel.All;
            ProfilerHelpers.LaunchAppAndCompareResult(testApp: "InstrumentationMethodLoggingTests_Debug_x64", fileName: "InstrumentationMethodLoggingAll.xml", args: null, regexCompare: true);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void InstrMethodLoggingTestLevelSome()
        {
            TestParameters.DisableMethodPrefix = false;
            TestParameters.MethodLogLevel = LogLevel.Errors | LogLevel.InstrumentationResults;
            ProfilerHelpers.LaunchAppAndCompareResult(testApp: "InstrumentationMethodLoggingTests_Debug_x64", fileName: "InstrumentationMethodLoggingSome.xml", args: null, regexCompare: true);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void InstrMethodLoggingTestLevelAllWithNoLogLevel()
        {
            TestParameters.DisableMethodPrefix = false;
            TestParameters.DisableLogLevel = true;
            TestParameters.MethodLogLevel = LogLevel.All;
            ProfilerHelpers.LaunchAppAndCompareResult(testApp: "InstrumentationMethodLoggingTests_Debug_x64", fileName: "InstrumentationMethodLoggingAllWithNoLogLevel.xml", args: null, regexCompare: true);
        }
    }
}
