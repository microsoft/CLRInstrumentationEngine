using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

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
    public class TestMultiReturn
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void MultiReturn_IfTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("MultiReturnTests_Release_x64", "MultiReturn_IfTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void MultiReturn_ExceptionTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("MultiReturnTests_Release_x64", "MultiReturn_ExceptionTest.xml");
        }

        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void MultiReturn_SwitchTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("MultiReturnTests_Release_x64", "MultiReturn_SwitchTest.xml");
        }
    }
}
