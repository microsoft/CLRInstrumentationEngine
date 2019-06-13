// Copyright (c) Microsoft Corporation. All rights reserved.
//

using System;

namespace TestRuntimeExceptionCallbacks
{
    class Program
    {
        static int Main(string[] args)
        {
            ThrowSomeExceptions1();
            return 0;
        }

        static void ThrowSomeExceptions1()
        {
            int i = 0;

            try
            {
                ThrowSomeExceptions2();
            }
            catch
            {
                i++;
            }
            finally
            {
                i++;
            }
        }

        static void ThrowSomeExceptions2()
        {
            int i = 0;

            try
            {
                ThrowSomeExceptions3();
            }
            catch (NullReferenceException)
            {
                i++;
            }
            finally
            {
                i++;
            }
        }

        static void ThrowSomeExceptions3()
        {
            int i = 0;

            try
            {
                i = 1 / i;
            }
            finally
            {
                i++;
            }
        }
    }
}
