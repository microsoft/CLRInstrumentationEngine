namespace ApplicationInsightsCompatibility
{
    internal static class TestFiles
    {
        private const string InstrumentationEngineProfilerModuleName64 = "MicrosoftInstrumentationEngine_x64.dll";
        private const string InstrumentationEngineDefaultMethodModuleName64 = "Microsoft.InstrumentationEngine.Extensions.Base_x64.dll";

        private const string InstrumentationEngineProfilerModuleName32 = "MicrosoftInstrumentationEngine_x86.dll";
        private const string InstrumentationEngineDefaultMethodModuleName32 = "Microsoft.InstrumentationEngine.Extensions.Base_x86.dll";

        private const string MscorlibExtensionMethodsBaseModuleName = "Microsoft.Diagnostics.Instrumentation.Extensions.Base.dll";
        private const string InstrumentationEngineHostConfigName = "Microsoft.InstrumentationEngine.Extensions.config";

        public const string AgentInterceptModuleName = "Microsoft.AI.Agent.Intercept.dll";
        public const string TestFrameworkModuleName = "Microsoft.VisualStudio.TestPlatform.TestFramework.dll";

        private const string BasePathAnyCpu = "..\\..\\..\\";
        private const string BasePathX86 = "..\\..\\..\\..\\x86\\";
        private const string BasePathX64 = "..\\..\\..\\..\\x64\\";

        public const string DeploymentItem_InstrumentationEngineModule32 = BasePathX86 + InstrumentationEngineProfilerModuleName32;
        public const string DeploymentItem_InstrumentationEngineModule64 = BasePathX64 + InstrumentationEngineProfilerModuleName64;

        public const string DeploymentItem_NativeExtensionsBaseModule32 = BasePathX86 + InstrumentationEngineDefaultMethodModuleName32;
        public const string DeploymentItem_NativeExtensionsBaseModule64 = BasePathX64 + InstrumentationEngineDefaultMethodModuleName64;

        public const string DeploymentItem_NativeExtensionsBaseConfig32 = BasePathX86 + InstrumentationEngineHostConfigName;
        public const string DeploymentItem_NativeExtensionsBaseConfig64 = BasePathX64 + InstrumentationEngineHostConfigName;

        public const string DeploymentItem_ManagedExtensionsBaseModule = BasePathAnyCpu + MscorlibExtensionMethodsBaseModuleName;

        public const string DeploymentItem_AgentInterceptModule = AgentInterceptModuleName;

        public const string DeploymentItem_TestFrameworkModule = AgentInterceptModuleName;

        public static string GetInstrumentationEngineProfilerModuleName(bool is32Bit) => is32Bit ?
            InstrumentationEngineProfilerModuleName32 :
            InstrumentationEngineProfilerModuleName64;
    }
}
