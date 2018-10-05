// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "ExtensionsCommon\InteropInstrumentationHandler.h"
#include "ExtensionsCommon\MethodInstrumentationInfoCollection.h"
#include "ExtensionsCommon\NativeInstanceMethodInstrumentationInfo.h"
#include "MethodInfoMock.h"

namespace TestsHostExtension
{
	TEST_CLASS(InteropInstrumentationHandlerTests)
	{
	public:
		BEGIN_TEST_METHOD_ATTRIBUTE(TestShouldInstrumentCalledTwiceWillNotFail)
			TEST_DESCRIPTION(L"Ensures you can call ShouldInstrumentTwice")
			TEST_OWNER(L"REDMOND\\sergkanz")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(TestShouldInstrumentCalledTwiceWillNotFail)
		{
			// Sergey Kanzhelev: this code needs to be completed with the mocks for IModuleMetadata and IAssemblyMetaData


			//Agent::Instrumentation::CMethodInstrumentationInfoCollectionSptr spInteropMethods;
			//Create(spInteropMethods);

			//Agent::Instrumentation::Native::CNativeInstanceMethodInstrumentationInfoSptr spInteropMethod;

			//Create(
			//	spInteropMethod,
			//	L"*",
			//	L"*",
			//	L"Microsoft.Diagnostics.Instrumentation.Extensions.Base.NativeMethods.Attach",
			//	4,
			//	0,
			//	reinterpret_cast<UINT_PTR>(nullptr),
			//	reinterpret_cast<UINT_PTR>(nullptr));

			//spInteropMethods->Add(spInteropMethod);

			//Agent::Interop::CInteropInstrumentationHandler handler(spInteropMethods);

			//IMethodInfoSptr spMethodInfo(new CMethodInfoMock(L"Attach", L"Microsoft.Diagnostics.Instrumentation.Extensions.Base.NativeMethods.Attach"));

			//Assert::IsTrue(handler.ShouldInstrument(spMethodInfo) == S_OK);
		}
	};
}