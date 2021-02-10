// Copyright (c) Microsoft Corporation. All rights reserved.
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
    public class MethodReplacement
    {
        private static TestContext Context;

        [ClassInitialize]
        public static void Initialize(TestContext context)
        {
            Context = context;
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

            ProfilerHelpers.LaunchAppAndCompareResult(
                TestParameters.FromContext(Context),
                "MethodReplacementTests_Release_x64",
                "Instru_MethodBaselineTest.xml");
        }
    }
}