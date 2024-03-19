// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.IO;
using System.Reflection;

namespace InstrEngineTests
{
    public static class PathUtils
    {
        private const string BaselinesFolder = "Baselines";
        private const string TestResultsFolder = "TestResults";
        private const string TestScriptsFolder = "TestScripts";
        private const string ResourcesFolder = "ExtractedResources";

        // All paths are relative to the AnyCPU binaries directory e.g. bin/Debug/AnyCPU/(framework)
        // Use forward slashes '/' to maintain compatibility between Windows and Linux
        public const string BaselinesBinPath = @"./" + BaselinesFolder;
        public const string TestScriptsBinPath = @"./" + TestScriptsFolder;
        public const string InstrumentationEngineX64BinPath = @"../../x64/MicrosoftInstrumentationEngine_x64.dll";
        public const string InstrumentationEngineX86BinPath = @"../../x86/MicrosoftInstrumentationEngine_x86.dll";
        public const string NaglerInstrumentationConfigX64BinPath = @"../../x64/NaglerInstrumentationMethod_x64.xml";
        public const string NaglerInstrumentationConfigX86BinPath = @"../../x86/NaglerInstrumentationMethod_x86.xml";
        public const string NaglerInstrumentationMethodX64BinPath = @"../../x64/NaglerInstrumentationMethod_x64.dll";
        public const string NaglerInstrumentationMethodX86BinPath = @"../../x86/NaglerInstrumentationMethod_x86.dll";
        public const string LinuxInstrumentationEngineX64BinPath = @"../../../../out/Linux/bin/x64.Debug/ClrInstrumentationEngine/libInstrumentationEngine.so";
        public const string LinuxNaglerInstrumentationConfigX64BinPath = @"../../../../out/Linux/bin/x64.Debug/ClrInstrumentationEngine/NaglerInstrumentationMethod/LinuxNaglerInstrumentationMethod.xml";
        public const string LinuxNaglerInstrumentationMethodX64BinPath = @"../../../../out/Linux/bin/x64.Debug/ClrInstrumentationEngine/libNaglerInstrumentationEngine.so";

        public static string GetAssetsPath()
        {
            return Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
        }

        public static string GetResourcesFolder()
        {
            return Path.Combine(GetAssetsPath(), ResourcesFolder);
        }

        public static string GetBaselinesPath()
        {
            return Path.Combine(GetAssetsPath(), BaselinesFolder);
        }

        public static string GetTestResultsPath()
        {
            return Path.Combine(GetAssetsPath(), TestResultsFolder);
        }

        /// <summary>
        /// Location of test scripts root folder
        /// </summary>
        public static string GetTestScriptsPath()
        {
            return Path.Combine(GetAssetsPath(), TestScriptsFolder);
        }
    }
}
