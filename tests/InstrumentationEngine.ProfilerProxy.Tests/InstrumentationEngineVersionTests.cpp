// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "InstrumentationEngineVersion.h"

using namespace ProfilerProxy;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InstrumentationEngineProfilerProxyTests
{
    TEST_CLASS(InstrumentationEngineVersionTests)
    {
    private:
        InstrumentationEngineVersion* CreateInstrumentationEngineVersion(LPCWSTR wszVersion)
        {
            InstrumentationEngineVersion* pVersion;
            InstrumentationEngineVersion::Create(wszVersion, &pVersion);
            Assert::IsNotNull(pVersion);

            return pVersion;
        }

        void AssertSucceeded(HRESULT hr)
        {
            Assert::IsTrue(SUCCEEDED(hr));
        }

        void AssertFailed(HRESULT hr)
        {
            Assert::IsTrue(FAILED(hr));
        }

    public:
        // Tests creating InstrumentationEngineVersion
        // with valid version succeeds.
        TEST_METHOD(CreateValidVersion)
        {
            std::wstring validVersionsArray[] = {
                L"1.0.0",                   // public release
                L"1.0.0_Debug",             // public debug
                L"1.0.0-build00001",        // preview release
                L"1.0.0-build00001_Debug"   // preview debug
            };

            InstrumentationEngineVersion* pActualVersion = nullptr;
            for (auto validVersionStr : validVersionsArray)
            {
                AssertSucceeded(InstrumentationEngineVersion::Create(validVersionStr, &pActualVersion));
                Assert::IsTrue(validVersionStr.compare(pActualVersion->GetVersionString()) == 0);
            }
        }

        // Tests creating InstrumentationEngineVersion
        // with invalid version fails.
        TEST_METHOD(CreateInvalidVersion)
        {
            std::wstring invalidVersionsArray[] = {
                L"",                    // empty
                L"A",                   // not digits
                L"-1",                  // no minor/build
                L"1.0.0_debug"          // case sensitivity
                L"1.0.0-build"          // missing preview version
                L"1.0.0_Debug-build123" // out of order
            };

            InstrumentationEngineVersion* pActualVersion = nullptr;
            for (auto invalidVersion : invalidVersionsArray)
            {
                AssertFailed(InstrumentationEngineVersion::Create(invalidVersion, &pActualVersion));
                Assert::IsNull(pActualVersion);
            }
        }

        // Tests comparing InstrumentationEngineVersion
        // works between preview and release
        TEST_METHOD(CompareVersion)
        {
            // Base version 1.0.0
            InstrumentationEngineVersion* pBaseVersion = CreateInstrumentationEngineVersion(L"1.0.0");

            // Preview versions
            InstrumentationEngineVersion* pBasePreviewVersion1 = CreateInstrumentationEngineVersion(L"1.0.0-build1");
            InstrumentationEngineVersion* pBasePreviewVersion10 = CreateInstrumentationEngineVersion(L"1.0.0-build10");
            InstrumentationEngineVersion* pBasePreviewVersion01 = CreateInstrumentationEngineVersion(L"1.0.0-build01");
            InstrumentationEngineVersion* pHigherPreviewVersion1 = CreateInstrumentationEngineVersion(L"2.0.0-build1");

            // Normal string compare:
            // * if one str is substring of the other, the shorter one is "before",
            // * Otherwise, compares char codes one char at a time. This results in:
            // 1.0.0-build01 < 1.0.0-build1 < 1.0.0-build10
            Assert::IsTrue(pBasePreviewVersion01->Compare(*pBasePreviewVersion1) < 0);
            Assert::IsTrue(pBasePreviewVersion1->Compare(*pBasePreviewVersion10) < 0);

            // Preview versions are considered "before" release versions:
            // 1.0.0-build10 < 1.0.0
            Assert::IsTrue(pBasePreviewVersion10->Compare(*pBaseVersion) < 0);

            // higher preview semantic versions are "after" lower release versions
            // 1.0.0 < 2.0.0-build1
            Assert::IsTrue(pBaseVersion->Compare(*pHigherPreviewVersion1) < 0);
        }
    };
}