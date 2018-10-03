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
            object disableMethodSignatureValidationParameter =
                context.Properties["DisableMethodSignatureValidation"];

            bool disableMethodSignatureValidation = false;
            if (null != disableMethodSignatureValidationParameter &&
                Boolean.TryParse(disableMethodSignatureValidationParameter.ToString(),
                    out disableMethodSignatureValidation))
            {
                DisableMethodSignatureValidation = disableMethodSignatureValidation;
            }
        }

        #endregion

        #region Properties

        public static bool DisableMethodSignatureValidation { get; private set; }

        #endregion
    }
}
