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
    public class MethodReplacement
    {
        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            TestParameters.Initialize(context);
        }

        [Owner("wiktork")]
        [TestMethod]
        [Timeout(TestConstants.TestTimeout)]
        public void ReplaceMethodTest()
        {
            //0,0,3,3,6,6,a,a,e,e

            //Original IL
            //IL_0000 ldc.i4.s 0xc
            //IL_0002 stloc.0

            //IL_0003 ldc.i4.s 0xb
            //IL_0005 stloc.1

            //IL_0006 ldloc.0
            //IL_0007 ldloc.1
            //IL_0008 sub
            //IL_0009 stloc.0

            //IL_000a ldloc.0
            //IL_000b ret

            //New IL (baseline)
            //IL_0000 ldc.i4.s 0xc
            //IL_0002 stloc.0

            //IL_0003 ldc.i4.s 0xb
            //IL_0005 stloc.1

            //IL_0006 ldloc.0
            //IL_0007 ldloc.1
            //IL_0008 sub
            //IL_0009 stloc.0

            //<--Begin new instructions
            //IL_000a ldloc.0
            //IL_000b ldc.i4.1
            //IL_000c sub
            //IL_000d stloc.0

            //-->End new instructions
            //IL_000e ldloc.0
            //IL_000f ret

            ProfilerHelpers.LaunchAppAndCompareResult("MethodReplacementTests_Release_x64", "Instru_MethodBaselineTest.xml");
        }
    }
}