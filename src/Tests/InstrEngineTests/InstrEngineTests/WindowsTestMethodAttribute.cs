// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace InstrEngineTests
{
    /// <summary>
    /// Use this attribute on TestMethods that should only run on Windows.
    /// </summary>
    internal class WindowsTestMethodAttribute : TestMethodAttribute
    {
        public override TestResult[] Execute(ITestMethod testMethod)
        {
            if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                return new[] {
                    new TestResult()
                    {
                        Outcome = UnitTestOutcome.Inconclusive,
                         TestFailureException = new AssertInconclusiveException("Test only runs on Windows")

                    }};
            }

            return base.Execute(testMethod);
        }
    }
}
