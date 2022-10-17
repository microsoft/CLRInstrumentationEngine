// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "InstrumentationEngineDefs.h"
#include "InstrumentationEngineInfraPointerDefs.h"
#include "InstrumentationEnginePointerDefs.h"

//#include "ProfilerHostServices.h"

// AppDomain is UndefinedDomain for extensions loaded from config(not via Attach())
static const AppDomainID UndefinedDomain = 0;

class CInstrumentationMethodBase :
    public IInstrumentationMethod
    //public CommonLib::CModuleRefCount    
    //, virtual public CProfilerHostServices
{
public:
    CInstrumentationMethodBase(const CInstrumentationMethodBase&) = delete;
    CInstrumentationMethodBase() noexcept
        : m_sharedAppDomainId(0)
        , m_attachedAppDomainId(0)
    {}

    // IInstrumentationMethod
    virtual HRESULT STDMETHODCALLTYPE Initialize(
        _In_ IProfilerManager* pHost) final;

    virtual HRESULT STDMETHODCALLTYPE OnAppDomainCreated(
        _In_ IAppDomainInfo *pAppDomainInfo) final;

    virtual HRESULT STDMETHODCALLTYPE OnAppDomainShutdown(
        _In_ IAppDomainInfo *pAppDomainInfo) final;

    virtual HRESULT STDMETHODCALLTYPE OnAssemblyLoaded(
        _In_ IAssemblyInfo* pAssemblyInfo) final;
    virtual HRESULT STDMETHODCALLTYPE OnAssemblyUnloaded(
        _In_ IAssemblyInfo* pAssemblyInfo) final;

    virtual HRESULT STDMETHODCALLTYPE OnModuleLoaded(
        _In_ IModuleInfo* pModuleInfo) final;
    virtual HRESULT STDMETHODCALLTYPE OnModuleUnloaded(
        _In_ IModuleInfo* pModuleInfo) final;

    virtual HRESULT STDMETHODCALLTYPE OnShutdown() final;

    _Success_(return == 0)
    virtual HRESULT STDMETHODCALLTYPE ShouldInstrumentMethod(
        _In_ IMethodInfo* pMethodInfo,
        _In_ BOOL isRejit,
        _Out_ BOOL* pbInstrument) final;

    virtual HRESULT STDMETHODCALLTYPE BeforeInstrumentMethod(
        _In_ IMethodInfo* pMethodInfo,
        _In_ BOOL isRejit) final;

    virtual HRESULT STDMETHODCALLTYPE InstrumentMethod(
        _In_ IMethodInfo* pMethodInfo,
        _In_ BOOL isRejit) final;

    virtual HRESULT STDMETHODCALLTYPE OnInstrumentationComplete(
        _In_ IMethodInfo* pMethodInfo,
        _In_ BOOL isRejit) final;

    _Success_(return == 0)
    virtual HRESULT STDMETHODCALLTYPE AllowInlineSite(
        _In_ IMethodInfo* pMethodInfoInlinee,
        _In_ IMethodInfo* pMethodInfoCaller,
        _Out_ BOOL* pbAllowInline) final;

    virtual ~CInstrumentationMethodBase() {};

protected:

    virtual HRESULT InternalInitialize(
        _In_ const IProfilerManagerSptr& sptrHost);

    virtual HRESULT InternalOnAppDomainCreated(
        _In_ const IAppDomainInfoSptr& sptrAppDomainInfo);

    virtual HRESULT InternalOnAppDomainShutdown(
        _In_ const IAppDomainInfoSptr& sptrAppDomainInfo);

    virtual HRESULT InternalOnAssemblyLoaded(
        _In_ const IAssemblyInfoSptr& sptrAssemblyInfo);
    virtual HRESULT InternalOnAssemblyUnloaded(
        _In_ const IAssemblyInfoSptr& sptrAssemblyInfo);

    virtual HRESULT InternalOnModuleLoaded(
        _In_ const IModuleInfoSptr& sptrModuleInfo);
    virtual HRESULT InternalOnModuleUnloaded(
        _In_ const IModuleInfoSptr& sptrModuleInfo);

    virtual HRESULT InternalOnShutdown();

    virtual HRESULT InternalShouldInstrumentMethod(
        _In_ const IMethodInfoSptr& sptrMethodInfo,
        _In_ BOOL isRejit,
        _Out_ BOOL* pbInstrument);

    virtual HRESULT InternalBeforeInstrumentMethod(
        _In_ const IMethodInfoSptr& sptrMethodInfo,
        _In_ BOOL isRejit);

    virtual HRESULT InternalInstrumentMethod(
        _In_ const IMethodInfoSptr& sptrMethodInfo,
        _In_ BOOL isRejit);

    virtual HRESULT InternalOnInstrumentationComplete(
        _In_ const IMethodInfoSptr& sptrMethodInfo,
        _In_ BOOL isRejit);

    virtual HRESULT InternalAllowInlineSite(
        _In_ const IMethodInfoSptr& sptrMethodInfoInlinee,
        _In_ const IMethodInfoSptr& sptrMethodInfoCaller,
        _Out_ BOOL* pbAllowInline);

    // Returns id of app domain which attached current instance of extension.
    AppDomainID GetAttachedAppDomainId() const { return m_attachedAppDomainId; }

    // Returns shared domain id
    AppDomainID GetSharedDomainID() const { return m_sharedAppDomainId; }

    // Gets the appdomain id of the appdomain that attached this extension. Returns 0 for extensions loaded from config (not via Attach()).
    HRESULT GetCurrentThreadDomainID(_Out_ AppDomainID& appDomainId);

    HRESULT CheckCurrentThreadDomainIsAttachedDomain();

    HRESULT DoesMethodBelongToDomain(_In_ const IMethodInfoSptr& sptrMethodInfo);

private:
    AppDomainID m_attachedAppDomainId;
    AppDomainID m_sharedAppDomainId;
    ICorProfilerInfoQiSptr m_spICorProfilerInfo;
}; // CInstrumentationMethodBase