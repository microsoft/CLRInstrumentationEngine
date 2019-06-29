// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "CppUnitTest.h"
#include "CorHdr.h"
#include "SignatureValidator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InstrumentationEngineLibTests
{
    TEST_CLASS(SignatureValidatorTests)
    {
    public:

        TEST_METHOD(VerifyEmbededSignatureReturnsTueForMSTestHost)
        {
            TCHAR fullPath[MAX_PATH] = { 0 };
            GetModuleFileName(NULL, fullPath, MAX_PATH);

            CSignatureValidator validator;
            Assert::IsTrue(validator.VerifyEmbeddedSignature(fullPath));
        }


        TEST_METHOD(VerifyEmbededSignatureReturnsFalseForTestLibrary)
        {
            TCHAR fullPath[_MAX_PATH + 1];
            GetModuleFileName(GetModuleHandle(_T("InstrumentationEngine.Lib.Tests.dll")), fullPath, sizeof(fullPath) / sizeof(fullPath[0]));

            CSignatureValidator validator;
            Assert::IsFalse(validator.VerifyEmbeddedSignature(fullPath));
        }

    };
}