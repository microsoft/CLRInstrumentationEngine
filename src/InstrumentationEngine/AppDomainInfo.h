// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "DataContainer.h"
#include "AssemblyInfo.h"
#include "ModuleInfo.h"
#include <list>

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("1A52E2E8-D9CE-4F1D-BA77-8E5FD228EB98"))
    CAppDomainInfo : public IAppDomainInfo, public CDataContainer
    {
    private:
        CRITICAL_SECTION m_cs;

        AppDomainID m_appDomainId;

        // CAppDomainInfo is created early in the callback to AppDomainCreationStarted
        // This means most of the object is not ready at that time.
        // This flag is set to true once the final initialization is finished in FinishInitialization
        volatile bool m_bIsInitialized;

        bool m_bIsSystemDomain;
        bool m_bIsSharedDomain;
        bool m_bIsDefaultDomain;

        CComBSTR m_bstrAppDomainName;

        unordered_map<AssemblyID, CComPtr<CAssemblyInfo>> m_assemblyInfos;
        unordered_map<ModuleID, CComPtr<CModuleInfo>> m_moduleInfos;

    public:
        CAppDomainInfo(_In_ AppDomainID appDomainId);
        ~CAppDomainInfo();

        HRESULT FinishInitialization(_In_ ICorProfilerInfo* pRawICorProfilerInfo);

        HRESULT AddAssemblyInfo(_In_ CAssemblyInfo* pAssemblyInfo);
        HRESULT AssemblyInfoUnloaded(_In_ CAssemblyInfo* pAssemblyInfo);

        HRESULT AddModuleInfo(_In_ CModuleInfo* pModuleInfo);
        HRESULT ModuleInfoUnloaded(_In_ CModuleInfo* pModuleInfo);

        shared_ptr<list<AssemblyID>> GetAssemblyIds();
        shared_ptr<list<ModuleID>> GetModuleIds();

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CAppDomainInfo);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CAppDomainInfo);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IAppDomainInfo*>(this),
                static_cast<IDataContainer*>(static_cast<CDataContainer*>(this)),
                riid,
                ppvObject
                );
        }

        // IAppDomainInfo methods
    public:
        virtual HRESULT __stdcall GetAppDomainId(_Out_ AppDomainID* pAppDomainId) override;
        virtual HRESULT __stdcall GetIsSystemDomain(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsSharedDomain(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsDefaultDomain(_Out_ BOOL* pbValue) override;

        virtual HRESULT __stdcall GetName(_Out_ BSTR* pbstrName) override;

        virtual HRESULT __stdcall GetAssemblies(_Out_ IEnumAssemblyInfo** ppAssemblyInfos) override;
        virtual HRESULT __stdcall GetModules(_Out_ IEnumModuleInfo** ppModuleInfos) override;

        virtual HRESULT __stdcall GetAssemblyInfoById(_In_ AssemblyID assemblyID, _Out_ IAssemblyInfo** ppAssemblyInfo) override;
        virtual HRESULT __stdcall GetAssemblyInfosByName(_In_ BSTR pszAssemblyName, _Out_ IEnumAssemblyInfo** ppAssemblyInfos) override;

        virtual HRESULT __stdcall GetModuleCount(_Out_ ULONG* pcModuleInfos) override;
        virtual HRESULT __stdcall GetModuleInfoById(_In_ ModuleID moduleId, _Out_ IModuleInfo** ppModuleInfo) override;
        virtual HRESULT __stdcall GetModuleInfosByMvid(_In_ GUID mvid, _Out_ IEnumModuleInfo** ppModuleInfo) override;
        virtual HRESULT __stdcall GetModuleInfosByName(_In_ BSTR pszModuleName, _Out_ IEnumModuleInfo** ppModuleInfo) override;
    };
}