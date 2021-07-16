// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RemoteUnitTestExecutor
{
    public interface ITestBase
    {        
        void Initialize();

        void Execute();

        void TestCleanup();

        ITestResult TestResult { get; }
    }
}
