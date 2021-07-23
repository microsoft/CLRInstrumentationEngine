// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RemoteUnitTestExecutor
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics.CodeAnalysis;
    using System.IO;
    using System.Linq;
    using System.Runtime.Serialization;
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

        /// CA2300 is set to Info level, which will not cause a build break. However, RoslynAnalyzer and PostAnalysis
        /// pipeline tasks will fail on any level reported above None. Recommendation is to disable CA2300 and enable
        /// CA2301 and CA2302 as mitigations. However, these are part of the SDL ruleset and we should not modify its
        /// behavior. Thus, mitigate CA2301 and CA2302 and then suppress CA2300.
        [SuppressMessage("Security", "CA2300", Justification = "Mitigated with fixes for CA2301 and CA2302")]
        public static ITestResult CreateFromFile(string testOutputFileName)
        {
            using (FileStream deserializationStream = File.OpenRead(testOutputFileName))
            {
                BinaryFormatter formatter = new BinaryFormatter();
                formatter.Binder = new TestResultSerializationBinder();
                return (ITestResult)formatter.Deserialize(deserializationStream);
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

        private class TestResultSerializationBinder : SerializationBinder
        {
            private static IEnumerable<string> s_supportedTypes = new List<string>()
            {
                typeof(List<MethodInvocationInfo>).FullName,
                typeof(List<string>).FullName,
                typeof(MethodInvocationInfo).FullName,
                typeof(TestResult).FullName
            };

            public override Type BindToType(string assemblyName, string typeName)
            {
                if (s_supportedTypes.Contains(typeName, StringComparer.Ordinal))
                {
                    // Tells serializer to use type from deserialized data.
                    return null;
                }

                throw new NotSupportedException("Attempted to deserialize unexpected type: " + typeName);
            }
        }
    }
}