// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"

#include "InstrumentationMethodBase.h"
#include "AgentValidation.h"

#include "InstrumentationMethodUtils.h"

HRESULT CInstrumentationMethodBase::GetCurrentThreadDomainID(_Out_ AppDomainID& appDomainId)
{
	HRESULT hr = S_OK;
	ThreadID currentThread = 0;
    appDomainId = UndefinedDomain;

    if (FAILED(hr = m_spICorProfilerInfo->GetCurrentThreadID(&currentThread)))
    {
        return hr;
    }
    if (FAILED(hr = m_spICorProfilerInfo->GetThreadAppDomain(currentThread, &appDomainId)))
    {
        return hr;
    }

	return hr;
}

HRESULT CInstrumentationMethodBase::CheckCurrentThreadDomainIsAttachedDomain()
{
    HRESULT hr = S_OK;
    AppDomainID appDomainId = UndefinedDomain;

    IfFailRet(GetCurrentThreadDomainID(appDomainId));

    if (appDomainId == UndefinedDomain)
    {
        hr = S_OK;
    }
    else
    {
        hr = appDomainId == m_attachedAppDomainId ? S_OK : S_FALSE;
    }

    return hr;
}

HRESULT CInstrumentationMethodBase::DoesMethodBelongToDomain(
    _In_ const IMethodInfoSptr& spMethodInfo)
{
    IfTrueRet(nullptr == spMethodInfo, E_INVALIDARG);

    IModuleInfoSptr spModuleInfo;
    IfFailRet(spMethodInfo->GetModuleInfo(&spModuleInfo));

    IAppDomainInfoSptr spDomainInfo;
    IfFailRet(spModuleInfo->GetAppDomainInfo(&spDomainInfo));

    AppDomainID appDomainId = UndefinedDomain;
    IfFailRet(spDomainInfo->GetAppDomainId(&appDomainId));

    auto hr = ((appDomainId == m_attachedAppDomainId) || (appDomainId == m_sharedAppDomainId)) ? S_OK : S_FALSE;
	return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::Initialize(
	_In_ IProfilerManager* pHost)
{
	IfTrueRet(nullptr == pHost, E_INVALIDARG);
	IProfilerManagerSptr sptrIProfilerManager(pHost);
	IfTrueRet(nullptr == sptrIProfilerManager, E_INVALIDARG);

	auto hr = S_OK;

	IProfilerManagerLoggingSptr spLogger;
	IfFailRetHresult(sptrIProfilerManager->GetLoggingInstance(&spLogger), hr);
	this->SetLogger(spLogger);

	IUnknownSptr spIUnknown;
	IfFailRet(pHost->GetCorProfilerInfo(&spIUnknown));
    ICorProfilerInfoQiSptr spICorProfilerInfo = spIUnknown;
    if (nullptr == spICorProfilerInfo)
    {
        TraceError("Unable to initialize monitoring. Runtime Instrumentation Engine suports only FW 4.0+");
        return E_NOINTERFACE;
    }
    m_spICorProfilerInfo = spICorProfilerInfo;


    if (!SUCCEEDED(GetCurrentThreadDomainID(this->m_attachedAppDomainId)))
	{
		TraceMsg("Unable to get app domain id. This is normal case for extensions loaded from config (not via Attach())");
	}

    IAppDomainCollectionSptr spAppDomainCollection;
    IfFailRet(pHost->GetAppDomainCollection(&spAppDomainCollection));

    IEnumAppDomainInfoSptr spEnumAppDomainInfo;
    IfFailRet(spAppDomainCollection->GetAppDomains(&spEnumAppDomainInfo));

    ULONG ulAdFetched = 0;
    IAppDomainInfoSptr spAppDomainInfo;

    for (;
        SUCCEEDED(spEnumAppDomainInfo->Next(1, &spAppDomainInfo, &ulAdFetched))
        && ulAdFetched != 0
        && spAppDomainInfo != nullptr
        ;
    spAppDomainInfo.Detach())
    {
        BOOL isShared = FALSE;
        IfFailRet(spAppDomainInfo->GetIsSharedDomain(&isShared));

        if (isShared)
        {
            IfFailRet(spAppDomainInfo->GetAppDomainId(&m_sharedAppDomainId));
            break;
        }
    }

	IfFailRetHresult(this->InternalInitialize(sptrIProfilerManager), hr);

	return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnAppDomainCreated(
	_In_ IAppDomainInfo *pAppDomainInfo)
{
	IfTrueRet(nullptr == pAppDomainInfo, E_INVALIDARG);
	IAppDomainInfoSptr sptrAppDomainInfo(pAppDomainInfo);
	IfTrueRet(nullptr == sptrAppDomainInfo, E_INVALIDARG);

	HRESULT hr = S_OK;

	BOOL isShared = FALSE;
	IfFailRet(sptrAppDomainInfo->GetIsSharedDomain(&isShared));

	if (isShared)
	{
		IfFailRet(sptrAppDomainInfo->GetAppDomainId(&m_sharedAppDomainId));
	}

	IfFailRetHresult(this->InternalOnAppDomainCreated(sptrAppDomainInfo), hr);

	return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnAppDomainShutdown(
	_In_ IAppDomainInfo *pAppDomainInfo)
{
    IfTrueRet(nullptr == pAppDomainInfo, E_INVALIDARG);
	IAppDomainInfoSptr sptrAppDomainInfo(pAppDomainInfo);
	IfTrueRet(nullptr == sptrAppDomainInfo, E_INVALIDARG);

	HRESULT hr = S_OK;

	IfFailRetHresult(this->InternalOnAppDomainShutdown(sptrAppDomainInfo), hr);

	return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnAssemblyLoaded(
	_In_ IAssemblyInfo* pAssemblyInfo)
{
    IfTrueRet(nullptr == pAssemblyInfo, E_INVALIDARG);
	IAssemblyInfoSptr sptrAssemblyInfo(pAssemblyInfo);
	IfTrueRet(nullptr == sptrAssemblyInfo, E_INVALIDARG);

	HRESULT hr = S_OK;

	IfFailRetHresult(this->InternalOnAssemblyLoaded(sptrAssemblyInfo), hr);

	return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnAssemblyUnloaded(
	_In_ IAssemblyInfo* pAssemblyInfo)
{
    IfTrueRet(nullptr == pAssemblyInfo, E_INVALIDARG);
	IAssemblyInfoSptr sptrAssemblyInfo(pAssemblyInfo);
	IfTrueRet(nullptr == sptrAssemblyInfo, E_INVALIDARG);

	HRESULT hr = S_OK;

	IfFailRetHresult(this->InternalOnAssemblyUnloaded(sptrAssemblyInfo), hr);

	return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnModuleLoaded(
	_In_ IModuleInfo* pModuleInfo)
{
    IfTrueRet(nullptr == pModuleInfo, E_INVALIDARG);
	IModuleInfoSptr sptrModuleInfo(pModuleInfo);
	IfTrueRet(nullptr == sptrModuleInfo, E_INVALIDARG);

	HRESULT hr = S_OK;

	IfFailRetHresult(this->InternalOnModuleLoaded(sptrModuleInfo), hr);

	return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnModuleUnloaded(
	_In_ IModuleInfo* pModuleInfo)
{
    IfTrueRet(nullptr == pModuleInfo, E_INVALIDARG);
	IModuleInfoSptr sptrModuleInfo(pModuleInfo);
	IfTrueRet(nullptr == sptrModuleInfo, E_INVALIDARG);

	HRESULT hr = S_OK;

	IfFailRetHresult(this->InternalOnModuleUnloaded(sptrModuleInfo), hr);

	return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnShutdown()
{
    HRESULT hr = S_OK;

	hr = this->InternalOnShutdown();

	return hr;
}

_Success_(return == 0)
HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::ShouldInstrumentMethod(
	_In_ IMethodInfo* pMethodInfo,
	_In_ BOOL isRejit,
	_Out_ BOOL* pbInstrument)
{
    IfTrueRet(nullptr == pMethodInfo, E_INVALIDARG);
	IMethodInfoSptr sptrMethodInfo(pMethodInfo);
	IfTrueRet(nullptr == sptrMethodInfo, E_INVALIDARG);

	IfTrueRet(nullptr == pbInstrument, E_INVALIDARG);

	HRESULT hr = S_OK;
	IfFailRetHresult(IsValidMethodInfo(sptrMethodInfo), hr);
	if (S_FALSE == hr)
	{
		TraceMsg(L"Method with invalid token detected");
		return hr;
	}

	IfFailRetHresult(this->InternalShouldInstrumentMethod(sptrMethodInfo, isRejit, pbInstrument), hr);

	return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::BeforeInstrumentMethod(
	_In_ IMethodInfo* pMethodInfo,
	_In_ BOOL isRejit)
{
    IfTrueRet(nullptr == pMethodInfo, E_INVALIDARG);
	IMethodInfoSptr sptrMethodInfo(pMethodInfo);
	IfTrueRet(nullptr == sptrMethodInfo, E_INVALIDARG);

	IfFailRet(IsValidMethodInfo(sptrMethodInfo));

	HRESULT hr = S_OK;

	IfFailRetHresult(IsValidMethodInfo(sptrMethodInfo), hr);
	if (S_FALSE == hr)
	{
		TraceMsg(L"Method with invalid token detected");
		return hr;
	}

	IfFailRetHresult(this->InternalBeforeInstrumentMethod(sptrMethodInfo, isRejit), hr);

	return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::InstrumentMethod(
	_In_ IMethodInfo* pMethodInfo,
	_In_ BOOL isRejit)
{
    IfTrueRet(nullptr == pMethodInfo, E_INVALIDARG);
	IMethodInfoSptr sptrMethodInfo(pMethodInfo);
	IfTrueRet(nullptr == sptrMethodInfo, E_INVALIDARG);

	IfFailRet(IsValidMethodInfo(sptrMethodInfo));

	HRESULT hr = S_OK;

	IfFailRetHresult(IsValidMethodInfo(sptrMethodInfo), hr);
	if (S_FALSE == hr)
	{
		TraceMsg(L"Method with invalid token detected");
		return hr;
	}

	IfFailRetHresult(this->InternalInstrumentMethod(sptrMethodInfo, isRejit), hr);

	return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnInstrumentationComplete(
	_In_ IMethodInfo* pMethodInfo,
	_In_ BOOL isRejit)
{
    IfTrueRet(nullptr == pMethodInfo, E_INVALIDARG);
	IMethodInfoSptr sptrMethodInfo(pMethodInfo);
	IfTrueRet(nullptr == sptrMethodInfo, E_INVALIDARG);

	IfFailRet(IsValidMethodInfo(sptrMethodInfo));

	HRESULT hr = S_OK;

	IfFailRetHresult(IsValidMethodInfo(sptrMethodInfo), hr);
	if (S_FALSE == hr)
	{
		TraceMsg(L"Method with invalid token detected");
		return hr;
	}

	IfFailRetHresult(this->InternalOnInstrumentationComplete(sptrMethodInfo, isRejit), hr);

	return hr;
}

_Success_(return == 0)
HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::AllowInlineSite(
	_In_ IMethodInfo* pMethodInfoInlinee,
	_In_ IMethodInfo* pMethodInfoCaller,
	_Out_ BOOL* pbAllowInline)
{
    IfTrueRet(nullptr == pMethodInfoInlinee, E_INVALIDARG);
	IMethodInfoSptr sptrMethodInfoInlinee(pMethodInfoInlinee);
	IfTrueRet(nullptr == sptrMethodInfoInlinee, E_INVALIDARG);

	IfTrueRet(nullptr == pMethodInfoCaller, E_INVALIDARG);
	IMethodInfoSptr sptrMethodInfoCaller(pMethodInfoCaller);
	IfTrueRet(nullptr == sptrMethodInfoCaller, E_INVALIDARG);

	IfTrueRet(nullptr == pbAllowInline, E_INVALIDARG);

	HRESULT hr = S_OK;

	IfFailRetHresult(IsValidMethodInfo(sptrMethodInfoInlinee), hr);
	if (S_FALSE == hr)
	{
		TraceMsg(L"Method with invalid token detected: sptrMethodInfoInlinee");
		return hr;
	}

	IfFailRetHresult(IsValidMethodInfo(sptrMethodInfoCaller), hr);
	if (S_FALSE == hr)
	{
		TraceMsg(L"Method with invalid token detected: sptrMethodInfoCaller");
		return hr;
	}

	IfFailRetHresult(
		this->InternalAllowInlineSite(
			sptrMethodInfoInlinee,
			sptrMethodInfoCaller,
			pbAllowInline), hr);

	return hr;
}

HRESULT CInstrumentationMethodBase::InternalInitialize(
	_In_ const IProfilerManagerSptr&)
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT CInstrumentationMethodBase::InternalOnAppDomainCreated(
	_In_ const IAppDomainInfoSptr&)
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT CInstrumentationMethodBase::InternalOnAppDomainShutdown(
	_In_ const IAppDomainInfoSptr&)
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT CInstrumentationMethodBase::InternalOnAssemblyLoaded(
	_In_ const IAssemblyInfoSptr&)
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT CInstrumentationMethodBase::InternalOnAssemblyUnloaded(
	_In_ const IAssemblyInfoSptr&)
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT CInstrumentationMethodBase::InternalOnModuleLoaded(
	_In_ const IModuleInfoSptr&)
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT CInstrumentationMethodBase::InternalOnModuleUnloaded(
	_In_ const IModuleInfoSptr&)
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT CInstrumentationMethodBase::InternalOnShutdown()
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT CInstrumentationMethodBase::InternalShouldInstrumentMethod(
	_In_ const IMethodInfoSptr&,
	_In_ BOOL,
	_Out_ BOOL* shouldInstrument)
{
	HRESULT hr = S_OK;
	*shouldInstrument = FALSE;
	return hr;
}

HRESULT CInstrumentationMethodBase::InternalBeforeInstrumentMethod(
	_In_ const IMethodInfoSptr&,
	_In_ BOOL)
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT CInstrumentationMethodBase::InternalInstrumentMethod(
	_In_ const IMethodInfoSptr&,
	_In_ BOOL)
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT CInstrumentationMethodBase::InternalOnInstrumentationComplete(
	_In_ const IMethodInfoSptr&,
	_In_ BOOL)
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT CInstrumentationMethodBase::InternalAllowInlineSite(
	_In_ const IMethodInfoSptr&,
	_In_ const IMethodInfoSptr&,
	_Out_ BOOL* allowInline)
{
	HRESULT hr = S_OK;
	*allowInline = TRUE;
	return hr;
}