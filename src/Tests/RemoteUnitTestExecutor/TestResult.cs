﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RemoteUnitTestExecutor
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Runtime.Serialization.Formatters.Binary;

    /// <summary>
    ///     Class describing test execution.
    /// </summary>
    [Serializable]
    public class TestResult : ITestResult
    {
        public TestResult()
        {
            InvokedMethods = new List<MethodInvocationInfo>();
        }

        public IList<MethodInvocationInfo> InvokedMethods { get; set; }

        public bool Succeeded { get; set; }

        public string ExceptionString { get; set; }

        private List<string> ProfilerTraces = new List<string>();

        public void Serialize(string filename)
        {
            using (FileStream stream = File.Open(filename, FileMode.OpenOrCreate))
            {
                new BinaryFormatter().Serialize(stream, this);
            }
        }

        public static ITestResult CreateFromFile(string testOutputFileName)
        {
            using (FileStream deserializationStream = File.OpenRead(testOutputFileName))
            {
                return (ITestResult) new BinaryFormatter().Deserialize(deserializationStream);
            }
        }

        public void AddProfilerTraces(IEnumerable<string> tracesIterator)
        {
            foreach (var trace in tracesIterator)
            {
                this.ProfilerTraces.Add(trace);
            }
        }

        public IList<string> GetProfilerTraces()
        {
            return this.ProfilerTraces;
        }

        public string InvokedMethodsSequence
        {
            get
            {
                if (this.InvokedMethods != null)
                {
                    return this.InvokedMethods.Aggregate(
                        string.Empty,
                        (current, methodInvocationInfo) => current + "->" + methodInvocationInfo.MethodName);
                }

                return string.Empty;
            }
        }
    }
}