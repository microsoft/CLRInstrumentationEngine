// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "ProfilerHostServices.h"
#include "ICodeInjector.h"
#include "NativeInstanceMethodsCodeInjector.h"
#include "NativeInstanceMethodInstrumentationInfo.h"

namespace Agent
{
namespace Instrumentation
{
	HRESULT CNativeInstanceMethodsCodeInjector::Inject(
		_In_ const IMethodInfoSptr& sptrMethodInfo,
		_In_ const Native::CNativeInstanceMethodInstrumentationInfoSptr& spMethod)
	{
		IfTrueRet(nullptr == sptrMethodInfo, E_INVALIDARG);
		IfTrueRet(nullptr == spMethod, E_INVALIDARG);

		mdSignature mdSigToken = mdSignatureNil;
		IfFailRet(
			InternalBuildNativeMethodSignature(
				sptrMethodInfo,
				mdSigToken));

		//Create instructions
		IInstructionFactorySptr sptrInstructionFactory;
		IfFailRet(
			sptrMethodInfo->GetInstructionFactory(&sptrInstructionFactory));

		IInstructionGraphSptr sptrInstructionGraph;
		IfFailRet(sptrMethodInfo->GetInstructions(&sptrInstructionGraph));
		IfFailRet(sptrInstructionGraph->RemoveAll());

		IInstructionSptr spCurrentInstruction;
		IfFailRet(sptrInstructionGraph->GetFirstInstruction(&spCurrentInstruction));

		IfFailRet(InternalGenerateLoadNativeObjectAddressInstructions(
			sptrMethodInfo,
			sptrInstructionFactory,
			sptrInstructionGraph,
			spMethod->GetInstancePtr(),
			spCurrentInstruction));

		IfFailRet(InternalGenerateArgumentsLoadInstructions(
			sptrMethodInfo,
			sptrInstructionFactory,
			sptrInstructionGraph,
			spMethod->GetArgsCount(),
			spCurrentInstruction));

		IfFailRet(InternalGenerateLoadNativeObjectMethodAddressInstructions(
			sptrMethodInfo,
			sptrInstructionFactory,
			sptrInstructionGraph,
			spMethod->GetInstanceMethodPtr(),
			spCurrentInstruction));

		IInstructionSptr sptrCalliNativeMethod;
		IfFailRet(
			sptrInstructionFactory->CreateTokenOperandInstruction(
				Cee_Calli,
				mdSigToken,
				&sptrCalliNativeMethod));
		IfFailRet(
			sptrInstructionGraph->InsertAfter(
				spCurrentInstruction,
				sptrCalliNativeMethod));
		spCurrentInstruction = sptrCalliNativeMethod;

		IInstructionSptr sptrReturn;
		IfFailRet(
			sptrInstructionFactory->CreateInstruction(
				Cee_Ret,
				&sptrReturn));
		IfFailRet(
			sptrInstructionGraph->InsertAfter(
				spCurrentInstruction,
				sptrReturn));
		spCurrentInstruction = sptrReturn;

		return S_OK;
	}

	HRESULT CNativeInstanceMethodsCodeInjector::InternalBuildNativeMethodSignature(
		_In_ const IMethodInfoSptr& sptrMethodInfo,
		_Out_ mdSignature& mdSigToken)
	{
		// magical number. We assume there are no signature of unmanaged methods with more then 15 arguments
		const auto MaxSupportedSigSize = 20;
		// reserved minimal size of method signature
		const auto ReservedSingSize = 3;
		const auto MethodAgrsCountSigIndex = 1;
		const auto MethodRetValSigIndex = 2;

		const auto NumberOfExtraMethodParameters = 1;

#if defined(_M_X64)
		const auto NativeObjThisPointerElementType = ELEMENT_TYPE_I8;
#elif defined(_M_IX86)
		const auto NativeObjThisPointerElementType = ELEMENT_TYPE_I4;
#elif defined(_M_ARM64)
		const auto NativeObjThisPointerElementType = ELEMENT_TYPE_I8;
#else
		static_assert(true, "Unsupported processor architecture");
#endif

		// Get signature
		COR_SIGNATURE pSignature[MaxSupportedSigSize] = {};
		DWORD cbSignature = 0;
		IfFailRet(
			sptrMethodInfo->GetCorSignature(
			_countof(pSignature),
			pSignature,
			&cbSignature));
		IfTrueRet(cbSignature < ReservedSingSize, E_INVALIDARG);

		std::vector<COR_SIGNATURE> vecUpdatedSignature;
		// reserving extra space in the signature for [this] of native object
		vecUpdatedSignature.reserve((size_t)cbSignature + NumberOfExtraMethodParameters);

		// defining call convention as STDCALL
		vecUpdatedSignature.push_back(IMAGE_CEE_UNMANAGED_CALLCONV_STDCALL);
		// incrementing args count (inserting this argument for this)
		vecUpdatedSignature.push_back(
			pSignature[MethodAgrsCountSigIndex] + NumberOfExtraMethodParameters);
		// copy ret val type
		vecUpdatedSignature.push_back(pSignature[MethodRetValSigIndex]);
		// defining native object [this] as a parameter
		vecUpdatedSignature.push_back(NativeObjThisPointerElementType);
		// copying the rest of signature (all predefined method arguments)
		vecUpdatedSignature.insert(
			vecUpdatedSignature.end(),
			pSignature + ReservedSingSize,
			pSignature + cbSignature);

		// Get signature token
		IModuleInfoSptr sptrModuleInfo;
		IfFailRet(sptrMethodInfo->GetModuleInfo(&sptrModuleInfo));

		IMetaDataEmitSptr spIMetaDataEmit;
		IfFailRet(
			sptrModuleInfo->GetMetaDataEmit(
			reinterpret_cast<IUnknown**>(&spIMetaDataEmit)));
		IfFailRet(
			spIMetaDataEmit->GetTokenFromSig(
			&vecUpdatedSignature[0],
			static_cast<ULONG>(vecUpdatedSignature.size()),
			&mdSigToken));

		return S_OK;
	}

	HRESULT CNativeInstanceMethodsCodeInjector::InternalGenerateArgumentsLoadInstructions(
		_In_ const IMethodInfoSptr&,
		_In_ const IInstructionFactorySptr& spInstructionFactory,
		_In_ const IInstructionGraphSptr& spInstructionGraph,
		_In_ int iArgsCount,
		_Inout_ IInstructionSptr& spCurrentInstruction)
	{
		for (int idx = 0; idx < iArgsCount; ++idx)
		{
			IInstructionSptr sptrLoadArg;

			IfFailRet(
				spInstructionFactory->CreateLoadArgInstruction(
				idx,
				&sptrLoadArg));

			IfFailRet(
				spInstructionGraph->InsertAfter(
				spCurrentInstruction,
				sptrLoadArg));

			spCurrentInstruction = sptrLoadArg;
		}

		return S_OK;
	}

	HRESULT CNativeInstanceMethodsCodeInjector::InternalGenerateLoadNativeObjectAddressInstructions(
		_In_ const IMethodInfoSptr&,
		_In_ const IInstructionFactorySptr& spInstructionFactory,
		_In_ const IInstructionGraphSptr& spInstructionGraph,
		_In_ const UINT_PTR pInstance,
		_Inout_ IInstructionSptr& spCurrentInstruction)
	{
		IInstructionSptr spLoadNativeObjAddressInstruction;

#if defined(_M_X64)
		IfFailRet(
			spInstructionFactory->CreateLongOperandInstruction(
			Cee_Ldc_I8,
			pInstance,
			&spLoadNativeObjAddressInstruction));
#endif
#if defined(_M_IX86)
		IfFailRet(
			spInstructionFactory->CreateIntOperandInstruction(
			Cee_Ldc_I4,
			pInstance,
			&spLoadNativeObjAddressInstruction));
#endif
#if (!defined(_M_X64)) && !defined(_M_IX86)
		static_assert(true, "Unsupported processor architecture");
#endif

		IfFailRet(
			spInstructionGraph->InsertAfter(
			spCurrentInstruction,
			spLoadNativeObjAddressInstruction));

		spCurrentInstruction = spLoadNativeObjAddressInstruction;

		return S_OK;
	}

	HRESULT CNativeInstanceMethodsCodeInjector::InternalGenerateLoadNativeObjectMethodAddressInstructions(
		_In_ const IMethodInfoSptr&,
		_In_ const IInstructionFactorySptr& spInstructionFactory,
		_In_ const IInstructionGraphSptr& spInstructionGraph,
		_In_ const UINT_PTR pMethod,
		_Inout_ IInstructionSptr& spCurrentInstruction)
	{
		IInstructionSptr spLoadCallbackAddress;

#if defined(_M_X64)
		IfFailRet(
			spInstructionFactory->CreateLongOperandInstruction(
			Cee_Ldc_I8,
			pMethod,
			&spLoadCallbackAddress));
#endif
#if defined(_M_IX86)
		IfFailRet(
			spInstructionFactory->CreateIntOperandInstruction(
			Cee_Ldc_I4,
			pMethod,
			&spLoadCallbackAddress));
#endif
#if (!defined(_M_X64)) && !defined(_M_IX86)
		static_assert(true, "Unsupported processor architecture");
#endif

		IfFailRet(
			spInstructionGraph->InsertAfter(
			spCurrentInstruction,
			spLoadCallbackAddress));
		spCurrentInstruction = spLoadCallbackAddress;

		return S_OK;
	}

} // Instrumentation
} // Agent