// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Diagnostics;
using System.Runtime.CompilerServices;

namespace RemoteUnitTestExecutor
{
    public abstract class TestEngineBase
    {
        public ITestResult ExecuteTest(string testName, bool isX86 = false, string[] expectedErrors = null)
        {
            if (string.IsNullOrWhiteSpace(testName))
            {
                throw new ArgumentNullException(nameof(testName));
            }
            string testOutputFileName = string.Concat("testOutput_", Guid.NewGuid(), ".xml");
            ProcessStartInfo startInfo = new ProcessStartInfo
            {
                Arguments = "\"" + testName + "\" " + testOutputFileName,
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardInput = true,
                CreateNoWindow = true
            };
            if (!isX86)
            {
                startInfo.FileName = typeof(TestEngineBase).Assembly.Location;
            }
            else
            {
                startInfo.FileName = GetX86HostProcessName();
            }
            using Process debugee = new Process
            {
                StartInfo = startInfo
            };
            OnBeforeStarted(debugee);
            if (!debugee.Start())
            {
                throw new InvalidOperationException("Unable to create test container process");
            }
            string line = debugee.StandardOutput.ReadLine();
            TestResult testResult = null;
            try
            {
                Assert.AreEqual(line, "Done");
                testResult = TestResult.LoadFromFile(testOutputFileName);
                if (!testResult.Succeeded)
                {
                    Assert.Fail(testResult.ExceptionString);
                }
            }
            finally
            {
                debugee.StandardInput.WriteLine("");
                debugee.StandardInput.Flush();
                OnComplete(debugee, expectedErrors);
            }
            return testResult;
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private string GetX86HostProcessName()
        {
            return typeof(Host.Program).Assembly.Location;
        }

        protected virtual void OnBeforeStarted(Process debugee)
        {
        }

        protected virtual void OnComplete(Process debugee, string[] expectedErrors = null)
        {
        }
    }
}
