// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ExtensionsCommon/InstrumentationMethodBase.h"
#include "ExtensionsCommon/ICodeInjectorFwd.h"
#include "ExtensionsCommon/DecorationCallbacksInfoStaticReader.h"
#include "ExtensionsCommon/NativeInstanceMethodInstrumentationInfo.h"
#include "ExtensionsCommon/MethodInstrumentationInfoCollection.h"
#include "ExtensionsCommon/InteropInstrumentationHandler.h"


extern const GUID GUID_AppDomainIMList;

typedef CDataContainerAdapterImplT <
    std::vector<ATL::CComPtr<IInstrumentationMethod>>,
    IAppDomainInfoSptr,
    &GUID_AppDomainIMList> CAppDomainIMListDataContainerAdapter;


class CHostExtensionBase
	: public CInstrumentationMethodBase
{
public:
	CHostExtensionBase();
	virtual ~CHostExtensionBase();

	HRESULT InternalInitialize(
		_In_ const IProfilerManagerSptr& spHost) override final;

	_Success_(return == 0)
	HRESULT InternalShouldInstrumentMethod(
		_In_ const IMethodInfoSptr& spMethodInfo,
		_In_ BOOL isRejit,
		_Out_ BOOL* pbInstrument) override final;

	_Success_(return == 0)
	HRESULT InternalInstrumentMethod(
		_In_ const IMethodInfoSptr& spMethodInfo,
		_In_ BOOL isRejit) override final;

	HRESULT InternalOnModuleLoaded(
		_In_ const IModuleInfoSptr& spModuleInfo) override final;

	_Success_(return == 0)
	HRESULT InternalAllowInlineSite(
		_In_ const IMethodInfoSptr& spMethodInfoInlinee,
		_In_ const IMethodInfoSptr& spMethodInfoCaller,
		_Out_ BOOL* pbAllowInline) override final;

    HRESULT InternalOnAppDomainShutdown(
        _In_ const IAppDomainInfoSptr& spAppDomainInfo);

	virtual __int32 Attach(_In_ const __int64 modulePathPtr,
		_In_ const __int64 modulePtr,
		_In_ const __int64 classGuidPtr,
		_In_ const __int32 priority) final;

private:
	ICodeInjectorSPtr					m_spEnterLeaveExceptionCodeInjector;
	CDecorationCallbacksInfoStaticReaderSptr	m_spDecorationCallbacksInfoReader;
	IProfilerManagerSptr                m_spHost;

    Agent::Interop::CInteropInstrumentationHandlerUptr		m_spInteropHandler;


private:
	_Success_(return == 0)
	HRESULT GetModulePath(_Out_ std::wstring& modulePath) const;

}; // COperationExtensionImpl