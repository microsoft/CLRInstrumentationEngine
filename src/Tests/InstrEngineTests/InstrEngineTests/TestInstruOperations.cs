﻿// Copyright (c) Microsoft Corporation. All rights reserved.
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
    public class TestInstruOperations
    {
        private static TestContext Context;

        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            Context = context;
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_RemoveAllTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "InstruOperationsTests_Release_x64",
                "Instru_RemoveAllTest.xml",
                "Instru_CreateErrorTest");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_EmptyMethodRemoveAllTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "InstruOperationsTests_Release_x64",
                "Instru_EmptyMethodRemoveAllTest.xml",
                "Instru_EmptyMethodTest");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_ReplaceTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "InstruOperationsTests_Release_x64",
                "Instru_ReplaceTest.xml",
                "Instru_ReplaceTest");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_ReplaceTest2()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "InstruOperationsTests_Release_x64",
                "Instru_ReplaceTest2.xml",
                "Instru_CreateErrorTest");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_RemoveTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "InstruOperationsTests_Release_x64",
                "Instru_RemoveTest.xml",
                "Instru_RemoveTest");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_RemoveTest2()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "InstruOperationsTests_Release_x64",
                "Instru_RemoveTest2.xml",
                "Instru_CreateErrorTest");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void Instru_InsterAfterTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "InstruOperationsTests_Release_x64",
                "Instru_InsertAfterTest.xml",
                "Instru_InsertAfterTest");
        }
    }
}
