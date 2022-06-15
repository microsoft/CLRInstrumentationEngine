// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using InvalidCSharp;

namespace Entrypoint
{
    class Program
    {
        static int Main(string[] args)
        {
            Console.WriteLine("Beginning Validation...");

            var byRefLikeValidator = new ByRefLikeValidator();
            byRefLikeValidator.Validate();

            var refFieldValidator = new RefFieldValidator();
            refFieldValidator.Validate();

            Console.WriteLine("Finishing Validation...");

            return 0;
        }
    }
}
