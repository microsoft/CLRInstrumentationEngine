// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RemoteUnitTestExecutor
{
    using System;

    public class Program
    {
        public static void Main(string[] args)
        {
            if (args.Length < 2 || string.IsNullOrWhiteSpace(args[1]))
            {
                throw new ArgumentException("Wrong number of arguments provided - expecting at least 2 arguments:\r\n arg[0] - test name; \r\n arg[1] - test results file name");
            }

            string testTypeName = args[0];
            string testResultFileName = args[1];
            ITestBase testInstance = null;
            Exception exc = null;
            try
            {
                Type t = Type.GetType(testTypeName);
                testInstance = (TestBase) Activator.CreateInstance(t);

                testInstance.Initialize();

                testInstance.Execute();
            }
            catch (Exception e)
            {
                exc = e;
            }
            finally
            {
                try
                {
                    ITestResult result = null;
                    if (testInstance != null)
                    {
                        try
                        {
                            result = testInstance.TestResult;
                            testInstance.TestCleanup();
                        }
                        catch (Exception)
                        {
                            // TODO: indicate cleanup failure
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

                    result.Serialize(testResultFileName);
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