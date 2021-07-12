// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RemoteUnitTestExecutor
{
    /// <summary>
    ///     Base class for all tests.
    ///     Exposes test's lifecycle methods and TestResult.
    /// </summary>
    public abstract class TestBase : ITestBase
    {
        public TestBase()
        {
            TestResult = new TestResult();
        }

        public ITestResult TestResult { get; private set; }

        public void Initialize()
        {
            TestPreInitialize();
            TestInitialize();
        }

        public abstract void Execute();

        public virtual void TestCleanup()
        {
        }

        public virtual void TestPreInitialize()
        {
        }

        public virtual void TestInitialize()
        {
        }
    }
}