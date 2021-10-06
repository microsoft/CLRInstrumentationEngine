// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;

namespace LocalTests
{
    class CustomType
    {
    }

    class Program
    {
        static int error = 0;

        static int Main(string[] args)
        {
            if ((args.Length != 1) || string.IsNullOrEmpty(args[0]))
            {
                return 1;
            }

            Type t = typeof(Program);
            MethodInfo mi = t.GetMethod(args[0], BindingFlags.Static | BindingFlags.NonPublic);
            if (mi != null)
            {
                mi.Invoke(null, null);
            }
            else
            {
                error++;
            }

            return error;
        }

        private static void AddLocals()
        {
        }

        private static void AddLocalsExisting()
        {
            System.UInt16 a = 1;
            System.Exception e = new InvalidOperationException();
            System.DateTime d = DateTime.Now;

            Console.WriteLine(a);
            Console.WriteLine(e);
            Console.WriteLine(d);
        }
    }
}
