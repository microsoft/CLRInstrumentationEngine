// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Xml;
using System.Runtime.InteropServices;

namespace InstrEngineTests
{
    // Mirror the LoggingFlags in InstrumentationEngine.idl
    internal enum LogLevel
    {
        None = 0x0,
        Errors = 0x1,
        Trace = 0x2,
        InstrumentationResults = 0x4,
        All = 0x7,
        Unset = 0x8
    }

    internal class ProfilerHelpers
    {
        #region private fields
        private static readonly Guid ProfilerGuid = new Guid("{324F817A-7420-4E6D-B3C1-143FBED6D855}");

        private const string HostConfig32PathEnvName = "MicrosoftInstrumentationEngine_ConfigPath32_";
        private const string HostConfig64PathEnvName = "MicrosoftInstrumentationEngine_ConfigPath64_";

        private const string TestOutputEnvName = "Nagler_TestOutputPath";
        private const string TestScriptFileEnvName = "Nagler_TestScript";
        private const string TestOutputFileEnvName = "MicrosoftInstrumentationEngine_FileLogPath";
        private const string IsRejitEnvName = "Nagler_IsRejit";

        #endregion

        public const int TestAppTimeoutMs = 10000;

        // In order to debug the host process, set this to true and a messagebox will be thrown early in the profiler
        // startup to allow attaching a debugger.
        private static bool ThrowMessageBoxAtStartup = false;

        private static bool WaitForDebugger = false;

        private static bool BinaryRecompiled = false;

        // Enable ref recording to track down memory leaks. For debug only.
        private static bool EnableRefRecording = false;

        public static void LaunchAppAndCompareResult(string testApp, string fileName, string args = null, bool regexCompare = false, int timeoutMs = TestAppTimeoutMs)
        {
            // Usually we use the same file name for test script, baseline and test result
            ProfilerHelpers.LaunchAppUnderProfiler(testApp, fileName, fileName, false, args, timeoutMs);
            ProfilerHelpers.DiffResultToBaseline(fileName, fileName, regexCompare);
        }

        public static void LaunchAppUnderProfiler(string testApp, string testScript, string output, bool isRejit, string args, int timeoutMs = TestAppTimeoutMs)
        {
            if (!BinaryRecompiled)
            {
                BinaryRecompiled = true;
                TargetAppCompiler.DeleteExistingBinary(PathUtils.GetAssetsPath());
                TargetAppCompiler.ComplileCSharpTestCode(PathUtils.GetAssetsPath());
            }

            DeleteOutputFileIfExist(output);

            // Ensure test results path exists
            Directory.CreateDirectory(PathUtils.GetTestResultsPath());

            bool is32bitTest = Is32bitTest(testScript);
            string bitnessSuffix = is32bitTest ? "x86" : "x64";

            // TODO: call this only for 64bit OS
            SetBitness(is32bitTest);

            System.Diagnostics.ProcessStartInfo psi = new System.Diagnostics.ProcessStartInfo();
            psi.UseShellExecute = false;
            psi.EnvironmentVariables.Add("COR_ENABLE_PROFILING", "1");
            psi.EnvironmentVariables.Add("COR_PROFILER", ProfilerGuid.ToString("B", CultureInfo.InvariantCulture));
            psi.EnvironmentVariables.Add("COR_PROFILER_PATH", Path.Combine(PathUtils.GetAssetsPath(), string.Format(CultureInfo.InvariantCulture, "MicrosoftInstrumentationEngine_{0}.dll", bitnessSuffix)));

            if (EnableRefRecording)
            {
                psi.EnvironmentVariables.Add("EnableRefRecording", "1");
            }

            if (!TestParameters.DisableLogLevel)
            {
                psi.EnvironmentVariables.Add("MicrosoftInstrumentationEngine_LogLevel", "Dumps");
            }

            // Uncomment this line to debug tests
            //psi.EnvironmentVariables.Add("MicrosoftInstrumentationEngine_DebugWait", "1");

            if (ThrowMessageBoxAtStartup)
            {
                psi.EnvironmentVariables.Add("MicrosoftInstrumentationEngine_MessageboxAtAttach", @"1");
            }

            if (WaitForDebugger)
            {
                psi.EnvironmentVariables.Add("MicrosoftInstrumentationEngine_DebugWait", @"1");
            }

            if (TestParameters.DisableMethodSignatureValidation)
            {
                psi.EnvironmentVariables.Add("MicrosoftInstrumentationEngine_DisableCodeSignatureValidation", @"1");
            }

            if (TestParameters.DisableMethodPrefix)
            {
                psi.EnvironmentVariables.Add("MicrosoftInstrumentationEngine_DisableLogMethodPrefix", @"1");
            }

            if (TestParameters.MethodLogLevel != LogLevel.Unset)
            {
                StringBuilder methodLogLevelBuilder = new StringBuilder();
                if ((TestParameters.MethodLogLevel & LogLevel.All) == LogLevel.None)
                {
                    methodLogLevelBuilder.Append("|None");
                }
                else
                {
                    if ((TestParameters.MethodLogLevel & LogLevel.All) == LogLevel.All)
                    {
                        methodLogLevelBuilder.Append("|All");
                    }
                    else
                    {
                        if ((TestParameters.MethodLogLevel & LogLevel.Errors) == LogLevel.Errors)
                        {
                            methodLogLevelBuilder.Append("|Errors");
                        }

                        if ((TestParameters.MethodLogLevel & LogLevel.Trace) == LogLevel.Trace)
                        {
                            methodLogLevelBuilder.Append("|Messages");
                        }

                        if ((TestParameters.MethodLogLevel & LogLevel.InstrumentationResults) == LogLevel.InstrumentationResults)
                        {
                            methodLogLevelBuilder.Append("|Dumps");
                        }
                    }
                }

                psi.EnvironmentVariables.Add("MicrosoftInstrumentationEngine_LogLevel_D2959618-F9B6-4CB6-80CF-F3B0E3263888", methodLogLevelBuilder.ToString());
            }
            else
            {
                // This variable overrides ALL logging levels for FileLoggerSink.
                psi.EnvironmentVariables.Add("MicrosoftInstrumentationEngine_FileLog", "Dumps");
            }

            psi.EnvironmentVariables.Add(TestOutputEnvName, PathUtils.GetAssetsPath());
            psi.EnvironmentVariables.Add(
                is32bitTest ? HostConfig32PathEnvName : HostConfig64PathEnvName,
                Path.Combine(PathUtils.GetAssetsPath(), string.Format(CultureInfo.InvariantCulture, "NaglerInstrumentationMethod_{0}.xml", bitnessSuffix)));

            string scriptPath = Path.Combine(PathUtils.GetTestScriptsPath(), testScript);

            psi.EnvironmentVariables.Add(TestScriptFileEnvName, scriptPath);

            string outputPath = Path.Combine(PathUtils.GetTestResultsPath(), output);

            psi.EnvironmentVariables.Add(TestOutputFileEnvName, outputPath);
            psi.EnvironmentVariables.Add(IsRejitEnvName, isRejit ? "True" : "False");

            psi.FileName = Path.Combine(PathUtils.GetAssetsPath(), testApp);
            psi.Arguments = args;

            System.Diagnostics.Process testProcess = System.Diagnostics.Process.Start(psi);

            // test processes are short. They should not run for more than a few seconds.
            testProcess.WaitForExit(timeoutMs);

            try
            {
                // try to end the process in case it is running too long.
                testProcess.Kill();
            }
            catch
            {
                // failure expecected if the process is already ended.
            }

            Assert.AreEqual(0, testProcess.ExitCode, "Test application failed");
        }

        public static string[] SplitXmlDocuments(string content)
        {
            List<string> docs = new List<string>();
            const string xmlDeclarationString = "<?xml version=\"1.0\"?>";
            int idx = 0;
            int iNext = content.IndexOf(xmlDeclarationString, idx + 1, StringComparison.Ordinal);

            while (iNext != -1)
            {
                string doc = content.Substring(idx, iNext - idx);
                docs.Add(doc);
                idx = iNext;
                iNext = content.IndexOf(xmlDeclarationString, idx + 1, StringComparison.Ordinal);
            }

            iNext = content.Length;
            string lastDoc = content.Substring(idx, iNext - idx);
            docs.Add(lastDoc);

            return docs.ToArray();
        }

        public static void DiffResultToBaseline(string output, string baseline, bool regexCompare = false)
        {
            string outputPath = Path.Combine(PathUtils.GetTestResultsPath(), output);
            string baselinePath = Path.Combine(PathUtils.GetBaselinesPath(), baseline);

            try
            {
                string baselineStr;
                string outputStr;

                List<string> baselineStrList = new List<string>();
                List<string> outputStrList = new List<string>();

                using (StreamReader baselineStream = new StreamReader(baselinePath))
                using (StreamReader outputStream = new StreamReader(outputPath))
                {
                    string tmpStr;

                    StringBuilder strBuilder = new StringBuilder();
                    while ((tmpStr = baselineStream.ReadLine()) != null)
                    {
                        if (!string.IsNullOrEmpty(tmpStr))
                        {
                            strBuilder.Append(tmpStr);
                            baselineStrList.Add($"^{tmpStr}$");
                        }
                    }
                    baselineStr = strBuilder.ToString();

                    strBuilder = new StringBuilder();
                    while ((tmpStr = outputStream.ReadLine()) != null)
                    {
                        if (!string.IsNullOrEmpty(tmpStr) &&
                            !tmpStr.StartsWith("[TestIgnore]", StringComparison.Ordinal))
                        {
                            strBuilder.Append(tmpStr);
                            outputStrList.Add(tmpStr);
                        }
                    }
                    outputStr = strBuilder.ToString();
                }

                if (regexCompare)
                {
                    Assert.IsTrue(baselineStrList.Count == outputStrList.Count, "Baseline file line count does not match output file line count.");

                    for (int i = 0; i < baselineStrList.Count; i++)
                    {
                        Match match = Regex.Match(outputStrList[i], baselineStrList[i]);
                        if (!match.Success)
                        {
                            Assert.Fail("Baseline file regex failed to match output file:\n" + baselineStrList[i] + "\n" + outputStrList[i]);
                        }
                    }
                }
                else
                {
                    // Use XML comparison
                    string[] baselineDocs = SplitXmlDocuments(baselineStr);
                    string[] outputDocs = SplitXmlDocuments(outputStr);

                    Assert.AreEqual(baselineDocs.Length, outputDocs.Length);
                    for (int docIdx = 0; docIdx < baselineDocs.Length; docIdx++)
                    {
                        var baselineDocument = LoadXmlFromString(baselineDocs[docIdx]);
                        var outputDocument = LoadXmlFromString(outputDocs[docIdx]);

                        Assert.AreEqual(baselineDocument.ChildNodes.Count, outputDocument.ChildNodes.Count);

                        for (int i = 0; i < baselineDocument.ChildNodes.Count; i++)
                        {
                            XmlNode currBaselineNode = baselineDocument.ChildNodes[i];
                            XmlNode currOutputNode = outputDocument.ChildNodes[i];

                            DiffResultToBaselineNode(currBaselineNode, currOutputNode);
                        }
                    }
                }
            }
            catch (AssertFailedException)
            {
                Console.WriteLine($"Baseline FilePath: {baselinePath}");
                Console.WriteLine();
                Console.WriteLine($"Output FilePath: {outputPath}");
                throw;
            }
        }

        private static XmlDocument LoadXmlFromString(string inputXml)
        {
            // https://docs.microsoft.com/en-us/dotnet/fundamentals/code-analysis/quality-rules/ca3075#solution-3
            XmlDocument xmlDoc = new XmlDocument() { XmlResolver = null };
            using (var baselineStringReader = new StringReader(inputXml))
            {
                using (var xmlReader = XmlReader.Create(baselineStringReader, new XmlReaderSettings() { XmlResolver = null }))
                {
                    xmlDoc.Load(xmlReader);
                }
            }
            return xmlDoc;
        }

        private static void DiffResultToBaselineNode(XmlNode baselineNode, XmlNode outputNode)
        {
            const string VolatileAttribute = "Volatile";

            if (String.CompareOrdinal(baselineNode.Name, outputNode.Name) != 0)
            {
                Assert.Fail("Baseline node name does not equal output node name\n" + baselineNode.Name + "\n" + outputNode.Name);
            }

            bool isVolatile = baselineNode.Attributes != null &&
                baselineNode.Attributes[VolatileAttribute] != null &&
                string.Equals(baselineNode.Attributes[VolatileAttribute].Value, "True", StringComparison.OrdinalIgnoreCase);

            // Don't check values of nodes marked Volatile.
            // NOTE: Eventually this should also be made to do regexp matching against
            if (!isVolatile)
            {
                if (CompareOrdinalNormalizeLineEndings(baselineNode.Value, outputNode.Value) != 0)
                {
                    Assert.Fail("Baseline value does not equal output value\n" + baselineNode.Value + "\n" + outputNode.Value);
                }

                Assert.AreEqual(baselineNode.ChildNodes.Count, outputNode.ChildNodes.Count);

                for (int i = 0; i < baselineNode.ChildNodes.Count; i++)
                {
                    XmlNode currBaselineNode = baselineNode.ChildNodes[i];
                    XmlNode currOutputNode = outputNode.ChildNodes[i];

                    DiffResultToBaselineNode(currBaselineNode, currOutputNode);
                }
            }
        }

        // Do this because we are doing git autocrlf stuff so the baselines will have ???
        // but the test output files will always have Windows-style.
        private static int CompareOrdinalNormalizeLineEndings(string a, string b)
        {
            string normalA = a?.Replace("\r\n", "\n");
            string normalB = b?.Replace("\r\n", "\n");
            return String.CompareOrdinal(normalA, normalB);
        }

        private static XmlDocument LoadTestScript(string testScript)
        {
            string scriptPath = Path.Combine(PathUtils.GetTestScriptsPath(), testScript);

            using (var xmlReader = XmlReader.Create(scriptPath, new XmlReaderSettings() { XmlResolver = null }))
            {
                XmlDocument scriptDocument = new XmlDocument() { XmlResolver = null };
                scriptDocument.Load(xmlReader);
                return scriptDocument;
            }
        }

        private static bool Is32bitTest(string testScript)
        {
            const string ScriptRootElementName = "InstrumentationTestScript";
            const string WowAttribute = "Use32BitProfiler";

            XmlDocument scriptDocument = LoadTestScript(testScript);

            foreach (XmlNode child in scriptDocument.ChildNodes)
            {
                if (string.Equals(child.Name, ScriptRootElementName, StringComparison.Ordinal))
                {
                    bool is32bitTest = child.Attributes != null &&
                        child.Attributes[WowAttribute] != null &&
                        string.Equals(child.Attributes[WowAttribute].Value, "True", StringComparison.OrdinalIgnoreCase);

                    return is32bitTest;
                }
            }

            // The default is 64 bit test
            return false;
        }

        private static void DeleteOutputFileIfExist(string output)
        {
            string outputPath = Path.Combine(PathUtils.GetTestResultsPath(), output);

            if (File.Exists(outputPath))
            {
                File.Delete(outputPath);
            }
        }

        /// <summary>
        /// Specify the preferred bitness for launched test application
        /// </summary>
        private static void SetBitness(bool isWow64)
        {
            const int COMPLUS_ENABLE_64BIT = 0x00000001;

            int flag = NativeMethods.GetComPlusPackageInstallStatus();

            if (isWow64)
            {
                flag &= ~COMPLUS_ENABLE_64BIT;
            }
            else
            {
                flag |= COMPLUS_ENABLE_64BIT;
            }

            // safely ignore the result.
            _ = NativeMethods.SetComPlusPackageInstallStatus(flag);
        }
    }
}
