﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace InstrEngineTests
{
    public static class PathUtils
    {
        private const string BaselinesFolder = "Baselines";
        private const string TestResultsFolder = "TestResults";
        private const string TestScriptsFolder = "TestScripts";

        // All paths are relative to the AnyCPU binaries directory e.g. bin\Debug\AnyCPU
        public const string BaselinesBinPath = @".\" + BaselinesFolder;
        public const string TestScriptsBinPath = @".\" + TestScriptsFolder;
        public const string InstrumentationEngineX64BinPath = @"..\x64";
        public const string InstrumentationEngineX86BinPath = @"..\x86";
        public const string InstrumentationConfigX64BinPath = @"..\x64\NaglerInstrumentationMethod_x64.xml";
        public const string InstrumentationConfigX86BinPath = @"..\x86\NaglerInstrumentationMethod_x86.xml";
        public const string NaglerInstrumentationMethodX64BinPath = @"..\x64\NaglerInstrumentationMethod_x64.dll";
        public const string NaglerInstrumentationMethodX86BinPath = @"..\x86\NaglerInstrumentationMethod_x86.dll";

        public static string GetAssetsPath()
        {
            return Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
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

        /// <summary>
        /// Location of test binaries like Test Profiler Host
        /// </summary>
        public static string GetTestBinariesFolder(string bitnessSuffix)
        {
            string testPath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            return Path.Combine(testPath, @"..\" + bitnessSuffix);
        }

        /// <summary>
        /// Location of Instrumentation Engine
        /// </summary>
        public static string GetProductBinariesFolder(string bitnessSuffix)
        {
            string testPath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            return Path.Combine(testPath, @"..\" + bitnessSuffix);
        }

    }
}
