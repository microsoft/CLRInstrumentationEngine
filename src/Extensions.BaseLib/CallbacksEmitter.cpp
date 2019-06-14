// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"

#include "CallbacksEmitter.h"

#include "ExtensionsCommon/LoadArgumentsHelper.h"

#include "ExtensionsCommon/InstructionDefineExtensions.h"
#include "ExtensionsCommon/ReflectionHelper.h"
#include "ExtensionsCommon/WellKnownInstrumentationDefs.h"

#include <algorithm>

CCallbacksEmitter::CCallbacksEmitter(
	_In_ const CDecorationCallbacksInfoSptr& spCallbacks)
{
	IfTrueThrow(nullptr == spCallbacks, E_INVALIDARG);

	m_spCallbacksInfo = spCallbacks;
}

HRESULT CCallbacksEmitter::EmitModule(
	_In_ const IModuleInfoSptr& spModuleInfo)
{
	IfTrueRet(nullptr == spModuleInfo, E_INVALIDARG);

	ATL::CComBSTR bstrModuleName;
	IfFailRet(spModuleInfo->GetModuleName(&bstrModuleName));

	if (bstrModuleName != m_spCallbacksInfo->GetModuleName().c_str())
	{
        mdToken tkAssemblyRef = mdAssemblyRefNil;

        CReflectionHelperSptr spReflectionHelper;
        IfFailRet(CreateAndBuildUp(spReflectionHelper));

        IfFailRet(spReflectionHelper->GetAssemblyRefByName(spModuleInfo, ExtensionsBaseAssemblyName, tkAssemblyRef));

		if (mdAssemblyRefNil == tkAssemblyRef)
		{
			IfFailRet(
				this->InternalDefineCallbacksAssemblyRef(
					m_spCallbacksInfo,
					spModuleInfo,
					tkAssemblyRef));
		}

		if (mdAssemblyRefNil != tkAssemblyRef)
		{
			IMetaDataEmitSptr spIMetaDataEmit;
			IfFailRet(spModuleInfo->GetMetaDataEmit(
				reinterpret_cast<IUnknown**>(&spIMetaDataEmit)));

			mdTypeRef tkTypeRef = mdTypeRefNil;
			IfFailRet(spIMetaDataEmit->DefineTypeRefByName(
				tkAssemblyRef,
                ExtensionsBasePublicContractType,
				&tkTypeRef));

			IfFailRet(
				InternalDefineCallbackRefs(
				m_spCallbacksInfo->GetBeginCallbacks(),
				spIMetaDataEmit,
				tkTypeRef));

			IfFailRet(
				InternalDefineCallbackRefs(
				m_spCallbacksInfo->GetEndCallbacks(),
				spIMetaDataEmit,
				tkTypeRef));

			IfFailRet(
				InternalDefineCallbackRefs(
				m_spCallbacksInfo->GetExceptionCallbacks(),
				spIMetaDataEmit,
				tkTypeRef));
		}
	}

	return S_OK;
}

HRESULT CCallbacksEmitter::Inject(
	_In_ const IMethodInfoSptr& spMethodInfo,
    _In_ const CMethodRecordSptr& spMethodRecord)
{
	return S_OK;
}

_Success_(return == 0)
HRESULT CCallbacksEmitter::InternalDefineCallbacksAssemblyRef(
	_In_ const CDecorationCallbacksInfoSptr& spCallbacksInfo,
	_In_ const IModuleInfoSptr& spModuleInfo,
	_Out_ mdAssemblyRef& tkAssemblyRef)
{
	IMetaDataAssemblyEmitSptr spIMetaDataAssemblyEmit;
	IfFailRet(spModuleInfo->GetMetaDataAssemblyEmit(
		reinterpret_cast<IUnknown**>(&spIMetaDataAssemblyEmit)));

	IfFailRet(
		spIMetaDataAssemblyEmit->DefineAssemblyRef(
			&spCallbacksInfo->GetPublicKey()[0],
			static_cast<ULONG>(spCallbacksInfo->GetPublicKey().size()),
			spCallbacksInfo->GetAssemblyName().c_str(),
			&spCallbacksInfo->GetAssemblyMetadata(),
			nullptr,
			0,
			spCallbacksInfo->GetAssemblyFlags(),
			&tkAssemblyRef));

	return S_OK;
}

HRESULT CCallbacksEmitter::InternalDefineCallbackRefs(
	_In_ const DecorationCallbackInfoCollection& vecCallbacks,
	_In_ const IMetaDataEmitSptr& spIMetaDataEmit,
	mdTypeRef tkTypeRef)
{

	for (auto spCallbackInfo : vecCallbacks)
	{
		mdMemberRef tkMethodRef = mdMemberRefNil;
		IfFailRet(
			spIMetaDataEmit->DefineMemberRef(
			tkTypeRef,
			spCallbackInfo->GetMethodName().c_str(),
			&spCallbackInfo->GetMethodSig()[0],
			static_cast<ULONG>(spCallbackInfo->GetMethodSig().size()),
			&tkMethodRef));
	}

	return S_OK;
}
