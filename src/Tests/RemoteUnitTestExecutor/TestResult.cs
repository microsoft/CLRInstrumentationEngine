// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization.Formatters.Binary;

namespace RemoteUnitTestExecutor
{
    [Serializable]
    public sealed class TestResult : ITestResult
    {
        public TestResult()
        {
            Succeeded = true;
            InvokedMethods = new List<MethodInvocationInfo>();
        }

        public static TestResult LoadFromFile(string path)
        {
            using FileStream stream = File.Open(path, FileMode.Open);
            return (TestResult)new BinaryFormatter().Deserialize(stream);
        }

        public void SaveToFile(string path)
        {
            using FileStream stream = File.Open(path, FileMode.OpenOrCreate);
            new BinaryFormatter().Serialize(stream, this);
        }

        public string ExceptionString { get; set; }

        public IList<MethodInvocationInfo> InvokedMethods { get; }

        public string InvokedMethodsSequence
        {
            get
            {
                if (InvokedMethods != null)
                {
                    return InvokedMethods.Aggregate(string.Empty, (string current, MethodInvocationInfo methodInvocationInfo) => current + "->" + methodInvocationInfo.MethodName);
                }
                return string.Empty;
            }
        }

        public bool Succeeded { get; set; }
    }
}
