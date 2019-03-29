// Copyright (c) Microsoft Corporation. All rights reserved.
// 

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace InstrEngineTests
{
    public static class TestParameters
    {
        #region Methods

        public static void Initialize(TestContext context)
        {
            // The NaglerProfilerHost disregards code signing verification.
            // However, InstrumentationEngine now has a built-in ExtensionsHost
            // which requires code sign verification. For tests, we disable verification.
            DisableMethodSignatureValidation = true;
        }

        #endregion

        #region Properties

        public static bool DisableMethodSignatureValidation { get; private set; }

        #endregion
    }
}
