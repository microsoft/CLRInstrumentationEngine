// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "ExtensionsCommon/ICodeInjector.h"
#include "ExtensionsCommon/ProfilerHostServices.h"
#include "ExtensionsCommon/DecorationCallbacksInfo.h"

class CCallbacksEmitter final
	: public CProfilerHostServices
	, public ICodeInjector
{
	CCallbacksEmitter(const CCallbacksEmitter&) = delete;
public:
	CCallbacksEmitter(
		_In_ const CDecorationCallbacksInfoSptr& spCallbacks);

	HRESULT EmitModule(
		_In_ const IModuleInfoSptr& sptrModuleInfo) override final;

	HRESULT Inject(
		_In_ const IMethodInfoSptr& sptrMethodInfo,
        _In_ const CMethodRecordSptr& spMethodRecord) override final;

private:
	CDecorationCallbacksInfoSptr										m_spCallbacksInfo;

	_Success_(return == 0)
	HRESULT InternalDefineCallbacksAssemblyRef(
		_In_ const CDecorationCallbacksInfoSptr& spCallbacksInfo,
		_In_ const IModuleInfoSptr& sptrModuleInfo,
		_Out_ mdAssemblyRef& tkAssemblyRef);

	HRESULT InternalDefineCallbackRefs(
		_In_ const DecorationCallbackInfoCollection& vecCallbacks,
		_In_ const IMetaDataEmitSptr& spIMetaDataEmit,
		mdTypeRef tkTypeRef
		);
};

typedef std::shared_ptr<CCallbacksEmitter> CCallbacksEmitterSPtr;