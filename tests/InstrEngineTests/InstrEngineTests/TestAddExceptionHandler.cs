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
    [DeploymentItem(PathUtils.NaglerInstrumentationMethodX64BinPath)]
    [DeploymentItem(PathUtils.NaglerInstrumentationMethodX86BinPath)]
    public class TestAddExceptionHandler
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void ExceptionTinyMethodTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("AddExceptionHandlerTests_Release_x64", "AddExceptionHandler_ExceptionTinyMethodTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void ExceptionFatClauseTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("AddExceptionHandlerTests_Release_x64", "AddExceptionHandler_ExceptionFatMethodTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void ReturnMethodTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("AddExceptionHandlerTests_Release_x64", "AddExceptionHandler_ReturnMethodTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void ReturnVoidMethodTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("AddExceptionHandlerTests_Release_x64", "AddExceptionHandler_ReturnVoidMethodTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void ReturnAndThrowMethodTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("AddExceptionHandlerTests_Release_x64", "AddExceptionHandler_ReturnAndThrowMethodTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void MultiReturnToSingleReturnMethodTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("AddExceptionHandlerTests_Release_x64", "AddExceptionHandler_MultiReturnToSingleReturnMethodTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void MultiReturnToSingleReturnVoidMethodTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("AddExceptionHandlerTests_Release_x64", "AddExceptionHandler_MultiReturnToSingleReturnVoidMethodTest.xml");
        }
    }
}
