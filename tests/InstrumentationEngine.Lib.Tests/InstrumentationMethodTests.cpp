// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "CppUnitTest.h"
#include "CorHdr.h"
#include "InstrumentationMethod.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#pragma warning(disable:6387) // disable parameter should not be null check. We explicitly test for that error in this file.
namespace InstrumentationEngineLibTests
{
    TEST_CLASS(InstrumentationMethodTests)
    {
    public:

        TEST_METHOD(InitializeWithAllNullsReturnsInvalidArg)
        {
            MicrosoftInstrumentationEngine::CInstrumentationMethod method(nullptr, nullptr, nullptr, nullptr, GUID(), 0); // lgtm[cpp/callwithnullsal]
            Assert::AreEqual(E_INVALIDARG, method.Initialize(nullptr, false)); // lgtm[cpp/callwithnullsal]
        }

        TEST_METHOD(InitializeWithBadPathReturnsNotFound)
        {
            MicrosoftInstrumentationEngine::CInstrumentationMethod method(L"c:\\temp", L"instrumentation method", L"test method description", L"instrumentationMethod.dll", GUID(), 0);
            Assert::AreEqual(HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND), method.Initialize(nullptr, false)); // lgtm[cpp/callwithnullsal]
        }

        TEST_METHOD(InitializeWithNotSignedDllAndValidationOnReturnsFail)
        {
            TCHAR szPath[MAX_PATH + 1] = { 0 };
            GetModuleFileName(GetModuleHandle(L"InstrumentationEngine.Lib.Tests.dll"), szPath, sizeof(szPath) / sizeof(szPath[0]));

            std::wstring fullName(szPath);
            std::wstring fileName(PathFindFileName(szPath));

            std::wstring modulePath = std::wstring(szPath, 0, fullName.size() - fileName.size());


            MicrosoftInstrumentationEngine::CInstrumentationMethod method(BSTR(modulePath.c_str()), L"instrumentation method", L"test method description", BSTR(fileName.c_str()), GUID(), 0);
            Assert::AreEqual(E_FAIL, method.Initialize(nullptr, true)); // lgtm[cpp/callwithnullsal]
        }

        TEST_METHOD(InitializeWithNotSignedDllAndValidationOffReturnsFail)
        {
            TCHAR szPath[MAX_PATH + 1] = { 0 };
            GetModuleFileName(GetModuleHandle(L"InstrumentationEngine.Lib.Tests.dll"), szPath, sizeof(szPath) / sizeof(szPath[0]));

            std::wstring fullName(szPath);
            std::wstring fileName(PathFindFileName(szPath));

            std::wstring modulePath = std::wstring(szPath, 0, fullName.size() - fileName.size());


            MicrosoftInstrumentationEngine::CInstrumentationMethod method(BSTR(modulePath.c_str()), L"instrumentation method", L"test method description", BSTR(fileName.c_str()), GUID(), 0);
            Assert::AreEqual(HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND), method.Initialize(nullptr, false)); // lgtm[cpp/callwithnullsal]
        }

    };
}