// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace InstrEngineTests
{
    public static class TestParameters
    {
        #region Methods

        public static void Initialize(TestContext context)
        {
            // InstrumentationEngine uses a built-in default ExtensionsHost
            // which requires code sign verification. For tests, we disable verification.
            DisableMethodSignatureValidation = true;

            // InstrumentationEngine should by default log the InstrumentationMethod Guid as a prefix to every message.
            // For tests, this should be disabled unless we are explicitly testing the behavior.
            DisableMethodPrefix = true;

            // By default, this is not set.
            MethodLogLevel = LogLevel.Unset;

            // By default, enable LogLevel
            DisableLogLevel = false;

            TestContext = context;
        }

        #endregion

        #region Properties

        public static bool DisableMethodSignatureValidation { get; private set; }

        public static bool DisableMethodPrefix { get; internal set; }

        public static bool DisableLogLevel { get; internal set; }

        public static TestContext TestContext { get; internal set; }

        internal static LogLevel MethodLogLevel { get; set; }

        #endregion
    }
}
