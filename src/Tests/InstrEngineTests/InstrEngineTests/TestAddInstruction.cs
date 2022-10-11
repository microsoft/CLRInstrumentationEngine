// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Runtime.InteropServices;

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
    public class TestAddInstruction
    {
        private static TestContext Context;

        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            Context = context;
        }

        [WindowsTestMethod] // because of x86
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_IfTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Debug_x86",
                "AddNop_IfTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddMultiNop_IfTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Debug_x64",
                "AddMultiNop_IfTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddBranchTargets_IfTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Debug_x64",
                "AddBranchTargets_IfTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddBranchTargets_SwitchTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Release_x64",
                "AddBranchTargets_SwitchTest.xml");
        }

        [WindowsTestMethod] // because of x86
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_ForTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Debug_x86",
                "AddNop_ForTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_WhileTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Debug_x64",
                "AddNop_WhileTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_SwitchTest_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Debug_x64",
                "AddNop_SwitchTest_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_SwitchTest2_Debug()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Debug_x64",
                "AddNop_SwitchTest2_Debug.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_SwitchTest2()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests_Release_x64",
                "AddNop_SwitchTest2.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddNop_ArrayForeachTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests2_Release_x64",
                "AddNop_ArrayForeachTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddBranchTargets_ArrayForeachTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests2_Release_x64",
                "AddBranchTargets_ArrayForeachTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void TinyMethodTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "BasicManagedTests2_Release_x64",
                "TinyMethodTest.xml");
        }
    }
}
