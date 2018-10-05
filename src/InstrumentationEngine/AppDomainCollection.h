// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "DataContainer.h"
#include "AppDomainInfo.h"

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("3084ADC8-9792-4549-A068-D69EEB6E3AE8"))
    CAppDomainCollection : public IAppDomainCollection, public CDataContainer
    {
    private:
        CRITICAL_SECTION m_cs;

        // Locking: hold m_cs
        std::unordered_map<AppDomainID, CComPtr<CAppDomainInfo>> m_appDomains;

    public:
        CAppDomainCollection();
        ~CAppDomainCollection();

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CAppDomainCollection);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CAppDomainCollection);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IAppDomainCollection*>(this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }

    public:
        HRESULT AddAppDomainInfo(_In_ AppDomainID appdomainId, _In_ CAppDomainInfo* pAppDomainInfo);
        HRESULT RemoveAppDomainInfo(_In_ AppDomainID appdomainId);

        HRESULT OnShutdown();

        // IAppDomainCollection methods
    public:
        STDMETHOD(GetAppDomainCount)(_Out_ DWORD* pdwCount) override;
        STDMETHOD(GetAppDomainById)(_In_ AppDomainID appDomainId, _Out_ IAppDomainInfo** ppAppDomainInfo) override;
        STDMETHOD(GetAppDomainIDs)(_In_ DWORD cAppDomains, _Out_ DWORD* pcActual, _Out_writes_(cAppDomains) AppDomainID* AppDomainIDs) override;
        STDMETHOD(GetAppDomains)(_Out_ IEnumAppDomainInfo** ppEnumAppDomains) override;

        // NOTE: These helpers will search all appdomains
        STDMETHOD(GetAssemblyInfoById)(_In_ AssemblyID assemblyID, _Out_ IAssemblyInfo** ppAssemblyInfo) override;
        STDMETHOD(GetModuleInfoById)(_In_ ModuleID moduleID, _Out_ IModuleInfo** ppModuleInfo) override;
        STDMETHOD(GetModuleInfosByMvid)(_In_ GUID mvid, _Out_ IEnumModuleInfo** ppEnum) override;

        // Obtain a method info instance from a function id. This method info is for querying information only,
        // and cannot be used for instrumentation.
        STDMETHOD(GetMethodInfoById)(_In_ FunctionID functionID, _Out_ IMethodInfo** ppMethodInfo) override;
    };
}