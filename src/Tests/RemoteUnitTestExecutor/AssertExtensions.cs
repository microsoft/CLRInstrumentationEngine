// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RemoteUnitTestExecutor
{
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using System.Collections.Generic;

    public static class AssertExtensions
    {
        public static void ClearInvokedMethods(IList<MethodInvocationInfo> invokedMethods)
        {
            // Clear the methods
            invokedMethods.Clear();
            Assert.AreEqual(0, invokedMethods.Count, "Invoked methods are not cleared.");
        }

        public static void InvokedMethodsAreEqual(IList<MethodInvocationInfo> actual, string[] expectedMethodNames)
        {
            Assert.AreEqual(expectedMethodNames.Length, actual.Count, "Actual and expected invoked methods count mismatch.");

            for (int i = 0; i < expectedMethodNames.Length; ++i)
            {
                Assert.AreEqual(expectedMethodNames[i], actual[i].MethodName,
                    "Actual and expected invoked methods mismatch.");
            }
        }
    }
}
