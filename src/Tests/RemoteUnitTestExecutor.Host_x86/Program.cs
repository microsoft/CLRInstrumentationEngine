// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace RemoteUnitTestExecutor.Host
{
    public class Program
    {
        [LoaderOptimization(LoaderOptimization.MultiDomainHost)]
        public static void Main(string[] args)
        {
            Type.GetType("RemoteUnitTestExecutor.Program, RemoteUnitTestExecutor").GetMethod("Main").Invoke(null, new object[1] { args });
        }
    }
}
