// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "CppUnitTest.h"
#include "Util.h"
#include "CorHdr.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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
			MicrosoftInstrumentationEngine::CMetadataEnumCloser<CSimpleImplementation> closer(new CSimpleImplementation(), nullptr);
			closer.Get();

			closer.Reset(nullptr);
		}

	};
}