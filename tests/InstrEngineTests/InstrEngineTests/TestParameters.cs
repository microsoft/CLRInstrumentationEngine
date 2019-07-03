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
        }

        #endregion

        #region Properties

        public static bool DisableMethodSignatureValidation { get; private set; }

        #endregion
    }
}
