// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using InvalidCSharp;

// FOR TOMORROW - REVERT TO USING THE ORIGINAL IL FILES AND DEL'S VALIDATE CODE (HERE) -> THEN CLEAN UP THE IL TO REMOVE THE CASES WHERE WE VALIDATE EXCEPTIONS

namespace InvalidCSharp
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
