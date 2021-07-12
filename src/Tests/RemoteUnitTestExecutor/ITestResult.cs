// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RemoteUnitTestExecutor
{
    public interface ITestResult
    {
        bool Succeeded { get; }

        string ExceptionString { get; }
    }
}
