// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.IO;
using System.Reflection;

namespace InstrEngineTests
{
    public static class PathUtils
    {
        private const string BaselinesFolder = "Baselines";
        private const string DebugeesFolder = "Debugees";
        private const string TestResultsFolder = "TestResults";
        private const string TestScriptsFolder = "TestScripts";

        // All paths are relative to the AnyCPU binaries directory e.g. bin\Debug\AnyCPU
        public const string BaselinesBinPath = @".\" + BaselinesFolder;
        public const string TestScriptsBinPath = @".\" + TestScriptsFolder;
        public const string InstrumentationEngineX64BinPath = @"..\..\..\x64\MicrosoftInstrumentationEngine_x64.dll";
        public const string InstrumentationEngineX86BinPath = @"..\..\..\x86\MicrosoftInstrumentationEngine_x86.dll";
        public const string NaglerInstrumentationConfigX64BinPath = @"..\..\..\x64\Tests\NaglerInstrumentationMethod_x64.xml";
        public const string NaglerInstrumentationConfigX86BinPath = @"..\..\..\x86\Tests\NaglerInstrumentationMethod_x86.xml";
        public const string NaglerInstrumentationMethodX64BinPath = @"..\..\..\x64\Tests\NaglerInstrumentationMethod_x64.dll";
        public const string NaglerInstrumentationMethodX86BinPath = @"..\..\..\x86\Tests\NaglerInstrumentationMethod_x86.dll";

        public static string GetAssetsPath()
        {
            return Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
        }

        public static string GetBaselinesPath()
        {
            return Path.Combine(GetAssetsPath(), BaselinesFolder);
        }

        public static string GetDebugeesPath()
        {
            return Path.Combine(GetAssetsPath(), DebugeesFolder);
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
