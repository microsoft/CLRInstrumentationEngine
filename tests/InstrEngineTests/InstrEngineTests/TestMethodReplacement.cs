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
            // Note: despite the code having two locals, Roslyn will
            // optimize one of the locals out during compilation.

            //Original IL
            //IL_0000: ldc.i4.s 12
            //IL_0002: ldc.i4.s 11
            //IL_0004: stloc.0
            //IL_0005: ldloc.0
            //IL_0006: sub
            //IL_0007: ret

            //New IL (baseline)
            //IL_0000: ldc.i4.s 12
            //IL_0002: ldc.i4.s 11
            //IL_0004: stloc.0
            //IL_0005: ldloc.0
            //IL_0006: sub
            //<--Begin new instructions
            //IL_0007 stloc.0
            //IL_0008 ldloc.0
            //IL_0009 ldc.i4.1
            //IL_000a sub
            //IL_000b stloc.0
            //IL_000c ldloc.0
            //-->End new instructions
            //IL_000d ret

            ProfilerHelpers.LaunchAppAndCompareResult("MethodReplacementTests_Release_x64", "Instru_MethodBaselineTest.xml");
        }
    }
}