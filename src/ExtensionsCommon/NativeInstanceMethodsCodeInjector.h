// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "NativeInstanceMethodInstrumentationInfo.h"

namespace Agent
{
namespace Instrumentation
{

	class CNativeInstanceMethodsCodeInjector final
		: public CProfilerHostServices
	{
	public:
		HRESULT Inject(
			_In_ const IMethodInfoSptr& sptrMethodInfo,
			_In_ const Native::CNativeInstanceMethodInstrumentationInfoSptr& spMethod);

	private:
		HRESULT InternalBuildNativeMethodSignature(
			_In_ const IMethodInfoSptr& sptrMethodInfo,
			_Out_ mdSignature& mdSignature);

		HRESULT InternalGenerateArgumentsLoadInstructions(
			_In_ const IMethodInfoSptr& spMethodInfo,
			_In_ const IInstructionFactorySptr& spInstructionFactory,
			_In_ const IInstructionGraphSptr& spInstructionGraph,
			_In_ int iArgsCount,
			_Inout_ IInstructionSptr& spCurrentInstruction);

		HRESULT InternalGenerateLoadNativeObjectAddressInstructions(
			_In_ const IMethodInfoSptr& spMethodInfo,
			_In_ const IInstructionFactorySptr& spInstructionFactory,
			_In_ const IInstructionGraphSptr& spInstructionGraph,
			_In_ const UINT_PTR pInstance,
			_Inout_ IInstructionSptr& spCurrentInstruction);

		HRESULT InternalGenerateLoadNativeObjectMethodAddressInstructions(
			_In_ const IMethodInfoSptr& spMethodInfo,
			_In_ const IInstructionFactorySptr& spInstructionFactory,
			_In_ const IInstructionGraphSptr& spInstructionGraph,
			_In_ const UINT_PTR pMethod,
			_Inout_ IInstructionSptr& spCurrentInstruction);
	};

	typedef std::shared_ptr<CNativeInstanceMethodsCodeInjector> CNativeInstanceMethodsCodeInjectorSPtr;

} // Instrumentation
} // Agent