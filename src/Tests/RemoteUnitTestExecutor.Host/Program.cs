// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace RemoteUnitTestExecutor.Host
{
    public static class Program
    {
        public static void Main(string[] args)
        {
            Type t = Type.GetType("RemoteUnitTestExecutor.Program, RemoteUnitTestExecutor");
            var method = t.GetMethod("Main");
            method.Invoke(null, new object[] { args });
        }
    }
}