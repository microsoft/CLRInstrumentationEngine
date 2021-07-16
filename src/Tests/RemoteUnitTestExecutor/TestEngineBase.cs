// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RemoteUnitTestExecutor
{
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using System;
    using System.Diagnostics;
    using System.Runtime.CompilerServices;

    /// <summary>
    ///     Summary description for TestsExtensionHostLoading
    /// </summary>
    public class TestEngineBase
    {
        public ITestResult ExecuteTest(
            string testName,
            bool isX86 = false,
            string[] expectedErrors = null)
        {
            if (true == string.IsNullOrWhiteSpace(testName))
            {
                throw new ArgumentNullException("testName");
            }

            string testOutputFileName = "testOutput_" + Guid.NewGuid() + ".xml";

            var startInfo = new ProcessStartInfo
            {
                Arguments = "\"" + testName + "\" " + testOutputFileName,
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardInput = true,
                CreateNoWindow = true,
            };

            if (!isX86)
            {
                startInfo.FileName = typeof(TestEngineBase).Assembly.Location;
            }
            else
            {
                startInfo.FileName = this.GetX86HostProcessName();
            }


            using (var debugee = new Process { StartInfo = startInfo })
            {
                OnBeforeStarted(debugee);

                if (false == debugee.Start())
                {
                    throw new InvalidOperationException("Unable to create test container process");
                }

                var line = debugee.StandardOutput.ReadLine();

                ITestResult testResult = null;

                try
                {
                    Assert.AreEqual("Done", line);

                    testResult = TestResult.CreateFromFile(testOutputFileName);

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
        }

        protected virtual void OnComplete(Process debugee, string[] expectedErrors = null)
        {
        }

        protected virtual void OnBeforeStarted(Process debugee)
        {
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private string GetX86HostProcessName()
        {
            // this method should not be inlined so in case of x64 CLR will not attempt to load x86 dependency (Host)
            return typeof(RemoteUnitTestExecutor.Host.Program).Assembly.Location;
        }
    }
}