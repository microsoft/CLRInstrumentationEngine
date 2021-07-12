// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace RemoteUnitTestExecutor
{
    public class Program
    {
        [LoaderOptimization(LoaderOptimization.MultiDomainHost)]
        public static void Main(string[] args)
        {
            if (args.Length < 2 || string.IsNullOrWhiteSpace(args[1]))
            {
                throw new ArgumentException("Wrong number of arguments provided - expecting at least 2 arguments:\r\n arg[0] - test name; \r\n arg[1] - test results file name");
            }
            string testTypeName = args[0];
            string testResultFileName = args[1];
            TestBase testInstance = null;
            Exception exc = null;
            try
            {
                testInstance = (TestBase)Activator.CreateInstance(Type.GetType(testTypeName));
                testInstance.Initialize();
                testInstance.Execute();
            }
            catch (Exception ex)
            {
                exc = ex;
            }
            finally
            {
                try
                {
                    TestResult result = null;
                    if (testInstance != null)
                    {
                        try
                        {
                            result = testInstance.TestResult;
                            testInstance.TestCleanup();
                        }
                        catch (Exception)
                        {
                        }
                    }
                    else
                    {
                        result = new TestResult();
                    }
                    if (exc != null)
                    {
                        result.Succeeded = false;
                        result.ExceptionString = exc.Message + " " + exc.StackTrace;
                    }
                    else
                    {
                        result.Succeeded = true;
                    }

                    result.SaveToFile(testResultFileName);
                }
                finally
                {
                    Console.WriteLine("Done");
                    Console.In.ReadLine();
                }
            }
        }
    }
}
