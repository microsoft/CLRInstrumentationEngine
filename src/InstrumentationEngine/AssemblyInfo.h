// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "DataContainer.h"
#include "ModuleInfo.h"

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("6C8D8C14-8BCB-4773-BD0B-A1F7D4BED0E7"))
    CAssemblyInfo : public IAssemblyInfo, public CDataContainer
    {
        CRITICAL_SECTION m_cs;

        CComBSTR m_bstrAssemblyName;

        AssemblyID m_assemblyId;

        // NOTE: The manifest module must be loaded before this can be used.
        mdAssembly m_tkAssembly;

        ModuleID m_manifestModuleID;
        // the manifest module. Not set until the manifest module is added to the module info collection
        CComPtr<IModuleInfo> m_pManifestModule;

        // This is a circular reference between the appdomaininfo and the assembly info.
        // The circular reference is broken when the assembly is removed from the appdomain info's assembly
        // collection
        CComPtr<IAppDomainInfo> m_pAppDomainInfo;

        unordered_map<ModuleID, CComPtr<CModuleInfo>> m_moduleInfos;

        // Public key of the assembly. Not set until the manifest module has loaded.
        void const* m_pPublicKey;
        // Count of bytes in the public key. Not set until the manifest module has loaded.
        ULONG m_cbPublicKey;
        bool m_publicKeyTokenInitialized;
        CComBSTR m_bstrPublicKeyToken;


    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CAssemblyInfo);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CAssemblyInfo);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IAssemblyInfo*>(this),
                static_cast<IDataContainer*>(static_cast<CDataContainer*>(this)),
                riid,
                ppvObject
                );
        }

    public:

        CAssemblyInfo();
        ~CAssemblyInfo();

        HRESULT Initialize(
            _In_ AssemblyID assemblyID,
            _In_ WCHAR* wszAssemblyName,
            _In_ IAppDomainInfo* pAppDomainInfo,
            _In_ ModuleID manifestModuleID
            );

    public:
        HRESULT AddModuleInfo(_In_ CModuleInfo* pModuleInfo);
        HRESULT ModuleInfoUnloaded(_In_ CModuleInfo* pModuleInfo);

        // IAssemblyInfo methods
    public:
        virtual HRESULT __stdcall GetAppDomainInfo(_Out_ IAppDomainInfo** ppAppDomainInfo) override;

        virtual HRESULT __stdcall GetModuleCount(_Out_ ULONG* pcModuleInfos) override;
        virtual HRESULT __stdcall GetModules(_In_ ULONG cModuleInfos, _Out_ IEnumModuleInfo** pModuleInfos) override;
        virtual HRESULT __stdcall GetModuleById(_In_ ModuleID moduleId, _Out_ IModuleInfo** ppModuleInfo) override;
        virtual HRESULT __stdcall GetModuleByMvid(_In_ GUID* pMvid, _Out_ IModuleInfo** ppModuleInfo) override;
        virtual HRESULT __stdcall GetModulesByName(_In_ BSTR  pszModuleName, _Out_ IEnumModuleInfo** ppModuleInfo) override;

        virtual HRESULT __stdcall GetModuleForMethod(_In_ mdToken token, _Out_ IModuleInfo** ppModuleInfo) override;
        virtual HRESULT __stdcall GetModuleForType(_In_ BSTR pszTypeName, _In_ mdToken tkResolutionScope, _Out_ mdToken* pTkTypeDef) override;

        virtual HRESULT __stdcall GetManifestModule(_Out_ IModuleInfo** ppModuleInfo) override;
        virtual HRESULT __stdcall GetPublicKey(_In_ ULONG cbBytes, _Out_writes_(cbBytes) BYTE* pbBytes) override;
        virtual HRESULT __stdcall GetPublicKeySize(_Out_ ULONG* pcbBytes) override;

        virtual HRESULT __stdcall GetPublicKeyToken(_Out_ BSTR* pbstrPublicKeyToken) override;
        virtual HRESULT __stdcall GetID(_Out_ AssemblyID* pAssemblyId) override;
        virtual HRESULT __stdcall GetName(_Out_ BSTR* pbstrName) override;

        virtual HRESULT __stdcall GetMetaDataToken(_Out_ DWORD* pdwToken) override;

    private:
        HRESULT HandleManifestModuleLoad();
        HRESULT InitializePublicKeyToken();
    };
}

