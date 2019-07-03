// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reflection;

namespace InstruOperationsTests
{
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
        static void Instru_EmptyMethodTest()
        {
        }

        static void Instru_InsertAfterTest()
        {
            error++;

            int a = 5;
            switch (a)
            {
                case 1:
                case 2:
                case 3:
                    error++;
                    break;
                case 4:
                    error += 2;
                    break;
                default:
                    error--;
                    break;
            }
        }

        static void Instru_ReplaceTest()
        {
            int a = 5;
            for (int i = 0; i < a; ++i)
            {
                error++;
            }

            error -= 3;
        }

        static void Instru_RemoveTest()
        {
            while (true)
            {
                error++;
                if (error == 10)
                {
                    return;
                }
            }
        }

        static void Instru_CreateErrorTest()
        {
            int a = 5;

            try
            {
                for (int i = 0; i < a; ++i)
                {
                    error++;
                }

                throw new Exception("hello");
            }
            catch
            {
                error++;
            }
            finally
            {
                error++;
            }
        }
    }
}
