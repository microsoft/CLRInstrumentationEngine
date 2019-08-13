// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace ExceptionTests
{
    class Program
    {
        static int error = 0;

        static int Main(string[] args)
        {
            try
            {
                ExceptionTinyMethodTest();
            }
            catch { }

            try
            {
                ExceptionTinyMethodTest2();
            }
            catch { }

            try
            {
                ExceptionFatClauseTest();
            }
            catch { }

            try
            {
                ExceptionMultiTryCatchTest();
            }
            catch { }

            try
            {
                ExceptionFinallyTest();
            }
            catch { }

            return error;
        }

        static void ExceptionTinyMethodTest()
        {
            throw new Exception("Expected Exception");
        }

        /// <summary>
        /// Though small, this function won't be marked as Tiny by compiler
        /// This may because it contains a exception handler
        /// </summary>
        /// <returns></returns>
        static void ExceptionTinyMethodTest2()
        {
            try
            {
                throw new Exception("Expected Exception");
            }
            // Two small clauses
            catch (System.ApplicationException) { error += 1; }
            catch (System.SystemException) { error += 1; }

            error += 1;
        }

        static void ExceptionFatClauseTest()
        {
            try
            {
                throw new Exception("Expected Exception");
            }
            // Two small clauses
            catch (System.ApplicationException)
            {
                // This makes a fat exception clause
                string[] strArray = new string[] { "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1" };
                foreach (string s in strArray)
                {
                    if (s == "2")
                        error += 1;
                }
            }
            catch (System.SystemException) { error += 1; }

            error += 1;
        }

        static void ExceptionMultiTryCatchTest()
        {
            try
            {
                error += 1;
                throw new Exception("Expected Exception 1");
            }
            catch (System.ApplicationException)
            {
                try
                {
                    throw new Exception("Expected Exception 1-1");
                }
                catch (System.ApplicationException) { }
            }
            catch (Exception) { error -= 1; }

            try
            {
                error += 1;
                throw new Exception("Expected Exception 2");
            }
            // Two small clauses
            catch (System.ApplicationException) { error += 1; }
            catch (Exception) { error -= 1; }

            try
            {
                throw new Exception("Expected Exception 3");
            }
            // Two small clauses
            catch (System.ApplicationException) { error += 1; }
            catch (System.SystemException) { error += 1; }

            error += 1;
        }

        static void ExceptionFinallyTest()
        {
            try
            {
                error += 2;
                throw new Exception("Expected Exception 2");
            }
            // Two small clauses
            catch (System.ApplicationException) { error += 1; }
            catch (Exception) { error -= 1; }
            finally
            {
                error -= 1;
            }
        }
    }
}
