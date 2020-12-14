// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Extensions.BaseLib\ModuleMetaDataLoader.h"

#pragma warning( push )
#pragma warning( disable: 25143 ) // Writable function pointer 'pVoidStaticMethod' with fixed global memory address.
#pragma warning( disable: 25007 ) // member function '' can be static

namespace TestsHostExtension
{
    TEST_CLASS(ModuleMetaDataLoaderTests)
    {
    public:
        BEGIN_TEST_METHOD_ATTRIBUTE(TestSimpleLoading)
            TEST_DESCRIPTION(L"Verifies that module initializer can load module")
            TEST_OWNER(L"REDMOND\\sergkanz")
            END_TEST_METHOD_ATTRIBUTE()
            TEST_METHOD(TestSimpleLoading)
        {
            std::wstring strPath = L"..\\AnyCPU\\Microsoft.Diagnostics.Instrumentation.Extensions.Base.dll";
            CModuleMetaDataLoader loader(strPath);

            Assert::AreEqual<HRESULT>(S_OK, loader.Load());
            Assert::IsFalse(nullptr == loader.GetImageBaseAddress());
            Assert::IsFalse(nullptr == loader.GetMetaDataImport());

            mdTypeDef mdType;
            Assert::AreEqual<HRESULT>(S_OK, loader.GetMetaDataImport()->FindTypeDefByName(L"_System.Diagnostics.DebuggerHiddenAttribute", mdTokenNil, &mdType));

            Assert::IsFalse(mdType == mdTypeDefNil || mdType == mdTokenNil);
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(TestWrongPathWillRerturnNonOk)
            TEST_DESCRIPTION(L"Verifies that module initializer can load module")
            TEST_OWNER(L"REDMOND\\sergkanz")
            END_TEST_METHOD_ATTRIBUTE()
            TEST_METHOD(TestWrongPathWillRerturnNonOk)
        {
            std::wstring strPath = L"NonExisting.Extensions.Base.dll";
            CModuleMetaDataLoader loader(strPath);

            Assert::AreNotEqual<HRESULT>(S_OK, loader.Load());
            Assert::IsTrue(nullptr == loader.GetImageBaseAddress());
            Assert::IsTrue(nullptr == loader.GetMetaDataImport());
        }
    };
}
#pragma warning ( pop )