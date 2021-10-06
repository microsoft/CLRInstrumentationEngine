// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace InstrEngineTests
{
    public class TestParameters
    {
        #region Methods

        public static TestParameters FromContext(TestContext context)
        {
            return new TestParameters()
            {
                // InstrumentationEngine uses a built-in default ExtensionsHost
                // which requires code sign verification. For tests, we disable verification.
                DisableMethodSignatureValidation = true,

                // InstrumentationEngine should by default log the InstrumentationMethod Guid as a prefix to every message.
                // For tests, this should be disabled unless we are explicitly testing the behavior.
                DisableMethodPrefix = true,

                // By default, this is not set.
                MethodLogLevel = LogLevel.Unset,

                // By default, enable LogLevel
                DisableLogLevel = false,

                TestContext = context
            };
        }

        #endregion

        #region Properties

        public bool DisableMethodSignatureValidation { get; private set; }

        public bool DisableMethodPrefix { get; internal set; }

        public bool DisableLogLevel { get; internal set; }

        public TestContext TestContext { get; internal set; }

        internal LogLevel MethodLogLevel { get; set; }

        #endregion
    }
}
