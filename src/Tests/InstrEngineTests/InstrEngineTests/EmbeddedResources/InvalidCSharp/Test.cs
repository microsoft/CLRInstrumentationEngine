// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using InvalidCSharp;

namespace Test
{
    class Program
    {
        static int Main(string[] args)
        {
            Console.WriteLine("Begin");

            var brlv = new ByRefLikeValidator();
            brlv.Validate();

            Exec.GenericClass();

            var rfv = new RefFieldValidator();
            rfv.Validate();

            Console.WriteLine("End");

            return 0;
        }
    }
}
