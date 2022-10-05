// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RemoteUnitTestExecutor
{
    using System.Collections.Generic;

    public interface ITestResult
    {
        IList<MethodInvocationInfo> InvokedMethods { get; }

        IList<string> ProfilerTraces { get; }

        bool Succeeded { get; set; }

        string ExceptionString { get; set; }

        void Serialize(string filename);

        void AddProfilerTraces(IEnumerable<string> tracesIterator);

        string InvokedMethodsSequence { get; }
    }
}