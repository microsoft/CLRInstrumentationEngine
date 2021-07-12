// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace RemoteUnitTestExecutor
{
    public abstract class TestBase
    {
        protected TestBase()
        {
            TestResult = new TestResult();
        }

        public abstract void Execute();

        public void Initialize()
        {
            TestPreInitialize();
            TestInitialize();
        }

        public virtual void TestCleanup()
        {
        }

        protected virtual void TestInitialize()
        {
        }

        protected virtual void TestPreInitialize()
        {
        }

        public TestResult TestResult { get; }
    }
}
