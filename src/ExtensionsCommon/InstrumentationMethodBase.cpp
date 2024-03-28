// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "InstrumentationMethodBase.h"
//#include "AgentValidation.h"

#include "InstrumentationMethodUtils.h"

HRESULT CInstrumentationMethodBase::GetCurrentThreadDomainID(_Out_ AppDomainID& appDomainId)
{
    HRESULT hr = S_OK;

    return hr;
}

HRESULT CInstrumentationMethodBase::CheckCurrentThreadDomainIsAttachedDomain()
{
    HRESULT hr = S_OK;

    return hr;
}

HRESULT CInstrumentationMethodBase::DoesMethodBelongToDomain(
    _In_ const IMethodInfoSptr& spMethodInfo)
{
    auto hr = S_OK;
    return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::Initialize(
    _In_ IProfilerManager* pHost)
{

    auto hr = S_OK;

    return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnAppDomainCreated(
    _In_ IAppDomainInfo* pAppDomainInfo)
{

    HRESULT hr = S_OK;

    return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnAppDomainShutdown(
    _In_ IAppDomainInfo* pAppDomainInfo)
{

    HRESULT hr = S_OK;

    return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnAssemblyLoaded(
    _In_ IAssemblyInfo* pAssemblyInfo)
{

    HRESULT hr = S_OK;

    return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnAssemblyUnloaded(
    _In_ IAssemblyInfo* pAssemblyInfo)
{

    HRESULT hr = S_OK;

    return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnModuleLoaded(
    _In_ IModuleInfo* pModuleInfo)
{
    HRESULT hr = S_OK;


    return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnModuleUnloaded(
    _In_ IModuleInfo* pModuleInfo)
{

    HRESULT hr = S_OK;

    return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnShutdown()
{
    HRESULT hr = S_OK;

    return hr;
}

_Success_(return == 0)
HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::ShouldInstrumentMethod(
    _In_ IMethodInfo * pMethodInfo,
    _In_ BOOL isRejit,
    _Out_ BOOL * pbInstrument)
{

    HRESULT hr = S_OK;

    *pbInstrument = false;

    return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::BeforeInstrumentMethod(
    _In_ IMethodInfo * pMethodInfo,
    _In_ BOOL isRejit)
{

    HRESULT hr = S_OK;

    return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::InstrumentMethod(
    _In_ IMethodInfo * pMethodInfo,
    _In_ BOOL isRejit)
{

    HRESULT hr = S_OK;

    return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::OnInstrumentationComplete(
    _In_ IMethodInfo * pMethodInfo,
    _In_ BOOL isRejit)
{

    HRESULT hr = S_OK;

    return hr;
}

_Success_(return == 0)
HRESULT STDMETHODCALLTYPE CInstrumentationMethodBase::AllowInlineSite(
    _In_ IMethodInfo * pMethodInfoInlinee,
    _In_ IMethodInfo * pMethodInfoCaller,
    _Out_ BOOL * pbAllowInline)
{


    HRESULT hr = S_OK;
    *pbAllowInline = TRUE;
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
    _Out_ BOOL * shouldInstrument)
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
    _Out_ BOOL * allowInline)
{
    HRESULT hr = S_OK;
    *allowInline = TRUE;
    return hr;
}