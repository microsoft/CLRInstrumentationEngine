// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.IO;
using System.Reflection;

namespace Intercept.Tests
{
    internal static class TestFiles
    {
        private const string InstrumentationEngineProfilerModuleName64 = "MicrosoftInstrumentationEngine_x64.dll";
        private const string InstrumentationEngineDefaultMethodModuleName64 = "Microsoft.InstrumentationEngine.Extensions.Base_x64.dll";

        private const string InstrumentationEngineProfilerModuleName32 = "MicrosoftInstrumentationEngine_x86.dll";
        private const string InstrumentationEngineDefaultMethodModuleName32 = "Microsoft.InstrumentationEngine.Extensions.Base_x86.dll";

        private const string RawProfilerHookModuleName32 = "Microsoft.RawProfilerHook_x86.dll";
        private const string RawProfilerHookModuleName64 = "Microsoft.RawProfilerHook_x64.dll";

        private const string MscorlibExtensionMethodsBaseModuleName = "Microsoft.Diagnostics.Instrumentation.Extensions.Base.dll";
        private const string InstrumentationEngineHostConfigName = "Microsoft.InstrumentationEngine.Extensions.config";

        public const string AgentInterceptModuleName = "Microsoft.AI.Agent.Intercept.dll";
        public const string TestFrameworkModuleName = "Microsoft.VisualStudio.TestPlatform.TestFramework.dll";

        public const string Instrumentation32FolderName = "Instrumentation32";
        public const string Instrumentation64FolderName = "Instrumentation64";

        // Intercept tests are under a Intercept.<version>.Tests folder in the output directory.
#if INTERCEPT_TESTS
        private const string BasePathAnyCpu = "..\\..\\";
        private const string BasePathX86 = "..\\..\\..\\x86\\";
        private const string BasePathX64 = "..\\..\\..\\x64\\";
#else
        private const string BasePathAnyCpu = "..\\";
        private const string BasePathX86 = "..\\..\\x86\\";
        private const string BasePathX64 = "..\\..\\x64\\";
#endif

        public const string DeploymentItem_InstrumentationEngineModule32 = BasePathX86 + InstrumentationEngineProfilerModuleName32;
        public const string DeploymentItem_InstrumentationEngineModule64 = BasePathX64 + InstrumentationEngineProfilerModuleName64;

        public const string DeploymentItem_NativeExtensionsBaseModule32 = BasePathX86 + InstrumentationEngineDefaultMethodModuleName32;
        public const string DeploymentItem_NativeExtensionsBaseModule64 = BasePathX64 + InstrumentationEngineDefaultMethodModuleName64;

        public const string DeploymentItem_NativeExtensionsBaseConfig32 = BasePathX86 + InstrumentationEngineHostConfigName;
        public const string DeploymentItem_NativeExtensionsBaseConfig64 = BasePathX64 + InstrumentationEngineHostConfigName;

        public const string DeploymentItem_RawProfilerHookModule32 = BasePathX86 + RawProfilerHookModuleName32;
        public const string DeploymentItem_RawProfilerHookModule64 = BasePathX64 + RawProfilerHookModuleName64;

        public const string DeploymentItem_ManagedExtensionsBaseModule = BasePathAnyCpu + "net40\\" + MscorlibExtensionMethodsBaseModuleName;

        public const string DeploymentItem_AgentInterceptModule = AgentInterceptModuleName;

        public const string DeploymentTarget_Instrumentation32 = ".\\" + Instrumentation32FolderName;
        public const string DeploymentTarget_Instrumentation64 = ".\\" + Instrumentation64FolderName;

        public static string GetEngineModulePath(bool is32Bit) => is32Bit ?
            GetFullPath(Instrumentation32FolderName + "\\" + InstrumentationEngineProfilerModuleName32) :
            GetFullPath(Instrumentation64FolderName + "\\" + InstrumentationEngineProfilerModuleName64);

        public static string GetRawProfilerModulePath(bool is32Bit) => is32Bit ?
            GetFullPath(Instrumentation32FolderName + "\\" + RawProfilerHookModuleName32) :
            GetFullPath(Instrumentation64FolderName + "\\" + RawProfilerHookModuleName64);

        private static string GetFullPath(string moduleName)
        {
            var result = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), moduleName);
            Assert.IsTrue(File.Exists(result), $"File {result} doesn't exists");
            return result;
        }
    }
}
