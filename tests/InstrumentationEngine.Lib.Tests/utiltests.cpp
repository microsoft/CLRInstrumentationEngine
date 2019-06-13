// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "CppUnitTest.h"
#include "Util.h"
#include "SharedArray.h"
#include "CorHdr.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace MicrosoftInstrumentationEngine;

namespace InstrumentationEngineLibTests
{
    class CSimpleImplementation
    {
    public:
        unsigned long AddRef() { return 0; }

        unsigned long Release() { return 0; }

        void CloseEnum(HCORENUM hEnum) {}
    };

    TEST_CLASS(UtilTests)
    {
    public:

        TEST_METHOD(SimpleTest)
        {
            CMetadataEnumCloser<CSimpleImplementation> closer(new CSimpleImplementation(), nullptr);
            closer.Get();

            closer.Reset(nullptr);
        }

        TEST_METHOD(SharedArrayTest)
        {
            CSharedArray<int> arr(10);
            Assert::AreEqual((size_t)10, arr.Count());
            Assert::IsFalse(arr.IsEmpty());

            { // create a new scope so that the reset array will go out of scope.
                CSharedArray<int> arr2(arr);
                CSharedArray<int> arr3 = arr2;
                CSharedArray<int> arr4 = std::move(arr); // this should clean up arr

                Assert::AreEqual((size_t)10, arr2.Count());
                Assert::AreEqual((size_t)10, arr3.Count());
                Assert::AreEqual((size_t)10, arr4.Count());
                Assert::AreEqual(arr2.Get(), arr3.Get());
                Assert::AreEqual(arr3.Get(), arr4.Get());

                Assert::IsTrue(arr.IsEmpty());
                Assert::IsNull(arr.Get());
            }

            Assert::IsTrue(arr.IsEmpty());
            Assert::IsNull(arr.Get());
        }

    };
}