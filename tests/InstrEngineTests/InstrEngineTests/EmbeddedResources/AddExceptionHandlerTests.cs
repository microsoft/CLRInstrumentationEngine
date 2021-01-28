// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace AddExceptionHandlerTests
{
    class Program
    {
        static int error = 0;

        static int Main(string[] args)
        {
            try
            {
                throw new Exception();
            }
            catch (Exception) { }

            try
            {
                ExceptionTinyMethodTest();
            }
            catch (ApplicationException) { }

            try
            {
                ExceptionFatMethodTest();
            }
            catch (ApplicationException) { }

            ReturnMethodTest();

            ReturnVoidMethodTest();

            try
            {
                ReturnAndThrowMethodTest();
            }
            catch (ApplicationException) { }

            ReturnMultiMethodTest();

            ReturnMultiVoidMethodTest();

            return error;
        }

        static void ExceptionTinyMethodTest()
        {
            throw new ApplicationException("Expected Exception");
        }

        static void ExceptionFatMethodTest()
        {
            // This makes a fat method clause
            string[] strArray = new string[] { "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1" };
            foreach (string s in strArray)
            {
                if (s == "2")
                    error += 1;
            }

            throw new ApplicationException("Expected Exception");
        }

        static string ReturnMethodTest()
        {
            return "A";
        }

        static void ReturnVoidMethodTest()
        {
        }

        static string ReturnAndThrowMethodTest()
        {
            throw new ApplicationException("Expected Exception");
        }

        static string ReturnMultiMethodTest()
        {
            if (error == -1)
            {
                return "B";
            }
            return "A";
        }

        static void ReturnMultiVoidMethodTest()
        {
            if (error == -1)
            {
                return;
            }
            error = 0;
            return;
        }

    }
}
