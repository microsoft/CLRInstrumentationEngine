// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;

namespace InstruOperationsTests
{
    class Program
    {
        static int Main(string[] args)
        {
            int error = ReplacementMethodTest();
            return error;
        }

        private static int ReplacementMethodTest()
        {
            int a = 12;
            int b = 11;
            a -= b;
            return a;
        }
    }
}
