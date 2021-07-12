// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;

namespace RemoteUnitTestExecutor
{
    public static class AssertExtensions
    {
        public static void ClearInvokedMethods(IList<MethodInvocationInfo> invokedMethods)
        {
            invokedMethods.Clear();
            Assert.AreEqual<int>(0, invokedMethods.Count, "Invoked methods are not cleared.");
        }

        public static void InvokedMethodsAreEqual(IList<MethodInvocationInfo> actual, string[] expectedMethodNames)
        {
            Assert.AreEqual<int>(expectedMethodNames.Length, actual.Count, "Actual and expected invoked methods count mismatch.");
            for (int i = 0; i < expectedMethodNames.Length; i++)
            {
                Assert.AreEqual<string>(expectedMethodNames[i], actual[i].MethodName, "Actual and expected invoked methods mismatch.");
            }
        }
    }
}
