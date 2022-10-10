// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Runtime.InteropServices;
using Microsoft.VisualStudio.TestTools.UnitTesting;

// This file is only compiled for net70
namespace InstrEngineTests
{
    [TestClass]
    // "dotnet test" runs tests in-place (instead of copying them to a deployment
    // directory). DeploymentItems that copy from a source path to a destination
    // that is the same location are not necessary and cause file locking issues
    // when initializing the tests. Only use DeploymentItem for paths with different
    // destinations for .NET Core test assemblies.
    [DeploymentItem(PathUtils.InstrumentationEngineX64BinPath)]
    [DeploymentItem(PathUtils.InstrumentationEngineX86BinPath)]
    [DeploymentItem(PathUtils.NaglerInstrumentationConfigX64BinPath)]
    [DeploymentItem(PathUtils.NaglerInstrumentationConfigX86BinPath)]
    [DeploymentItem(PathUtils.NaglerInstrumentationMethodX64BinPath)]
    [DeploymentItem(PathUtils.NaglerInstrumentationMethodX86BinPath)]
    public class RefStructsTests
    {
        private static TestContext Context;

        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            Context = context;
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void TestRefStructs_Validate_RefStructWithRefField_Load()
        {
            if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows)) 
            {
                Assert.Inconclusive("RefStructs tests are only available on Windows");
            }

            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "RefStructsTests_Release_x64",
                "RefStructsTest_RefStructWithRefField_Load.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void TestRefStructs_Validate_Create_RefField()
        {
            if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows)) 
            {
                Assert.Inconclusive("RefStructs tests are only available on Windows");
            }

            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "RefStructsTests_Release_x64",
                "RefStructsTest_Create_RefField.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void TestRefStructs_Validate_Create_RefStructField()
        {
            if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows)) 
            {
                Assert.Inconclusive("RefStructs tests are only available on Windows");
            }

            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "RefStructsTests_Release_x64",
                "RefStructsTest_Create_RefStructField.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void TestRefStructs_Validate_Create_TypedReferenceRefField()
        {
            if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows)) 
            {
                Assert.Inconclusive("RefStructs tests are only available on Windows");
            }

            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "RefStructsTests_Release_x64",
                "RefStructsTest_Create_TypedReferenceRefField.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void TestRefStructs_Validate_Casting_Scenarios()
        {
            if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows)) 
            {
                Assert.Inconclusive("RefStructs tests are only available on Windows");
            }

            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "RefStructsTests_Release_x64",
                "RefStructsTest_Casting_Scenarios.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void TestRefStructs_Validate_RecognizedOpCodeSequences_Scenarios()
        {
            if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows)) 
            {
                Assert.Inconclusive("RefStructs tests are only available on Windows");
            }

            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "RefStructsTests_Release_x64",
                "RefStructsTest_RecognizedOpCodeSequences_Scenarios.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void TestRefStructs_Validate_Inlining_Behavior()
        {
            if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows)) 
            {
                Assert.Inconclusive("RefStructs tests are only available on Windows");
            }

            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "RefStructsTests_Release_x64",
                "RefStructsTest_Inlining_Behavior.xml");
        }
    }
}
