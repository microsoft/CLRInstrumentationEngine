// Copyright (c) Microsoft Corporation. All rights reserved.
//

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
    [DeploymentItem(PathUtils.TestInstrumentationMethodX64BinPath)]
    [DeploymentItem(PathUtils.TestInstrumentationMethodX86BinPath)]
    public class AddOperands
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        // Disabling test because AddOperandTest.xml test script does not exist
        //[TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void AddOperandTest()
        {
            ProfilerHelpers.LaunchAppAndCompareResult("BasicManagedTests_Debug_x64.exe", "AddOperandTest.xml", "AddOperandTest");
        }
    }
}
