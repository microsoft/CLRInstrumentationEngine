// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "DataContainer.h"
#include "MethodInfo.h"
#include "CachedILMethodBody.h"
#include "InlineSiteMap.h"
#include "../InstrumentationEngine.Lib/SharedArray.h"

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class CMethodInfo;
    class CMethodJitInfo;

    class __declspec(uuid("CDD3824F-B876-4450-9459-885BA1C21540"))
    CModuleInfo : public IModuleInfo3, public CDataContainer
    {
    private:
        class CMethodKey
        {
        public:

            mdMethodDef m_token;
            FunctionID m_functionID;

            CMethodKey(mdMethodDef token, FunctionID functionId) : m_token(token), m_functionID(functionId) {}

            bool operator==(const CMethodKey& other) const
            {
                return m_token == other.m_token && m_functionID == other.m_functionID;
            }

            struct Hash
            {
                size_t operator()(const CMethodKey& key) const
                {
                    return key.m_token ^ key.m_functionID;
                }
            };
        };
    private:
        CRITICAL_SECTION m_cs;

        ModuleID m_moduleID;
        CComBSTR m_bstrModulePath;
        CComBSTR m_bstrModuleName;
        GUID m_mvid;
        bool m_bIs64bit;
        bool m_bIsIlOnly;
        bool m_bIsMscorlib;
        bool m_bIsDynamic;
        bool m_bIsLoadedFromDisk;
        bool m_bIsNgen;
        bool m_bIsWinRT;
        bool m_bIsFlatLayout;

        const IMAGE_COR20_HEADER* m_pCorHeader;
        mdMemberRef m_tkEntrypoint;

        // Note circular reference to assembly info and appdomain info
        // broken when module unload is received
        CComPtr<IAssemblyInfo> m_pAssemblyInfo;
        CComPtr<IAppDomainInfo> m_pAppDomainInfo;

        LPCBYTE m_pModuleBaseLoadAddress;
        LPCBYTE m_pbSectionStart;
        WORD m_numSections;

        CAutoPtr<VS_FIXEDFILEINFO> m_pFixedFileVersion;

        CComPtr<IMetaDataImport2> m_pMetadataImport;
        CComPtr<IMetaDataAssemblyImport> m_pMetadataAssemblyImport;
        CComPtr<IMetaDataEmit2> m_pMetaDataEmit2;
        CComPtr<IMetaDataAssemblyEmit> m_pMetaDataAssemblyEmit;

        unordered_map<FunctionID, CComPtr<CMethodInfo>> m_methodInfos;
        unordered_map<mdMethodDef, CComPtr<CMethodInfo>> m_methodInfosByToken;

        unordered_map<mdToken, std::shared_ptr<CCachedILMethodBody>> m_methodTokenToCachedILMap;
        unordered_set<mdMethodDef> m_instrumentedMethods;
        unordered_map<CMethodKey, CSharedArray<COR_IL_MAP>, CMethodKey::Hash> m_ilMaps;

        CComPtr<ITypeCreator> m_pTypeFactory;

        // Map the method token to the number of times that method has rejitted.
        // Note this is not the same as the CLR's REJITID becase it is not functionid
        // specific
        unordered_map<mdToken, UINT32> m_methodTokenToRejitCountMap;

        // Map used to track where methods from this module have been inlined into.
        CComPtr<CInlineSiteMap> m_pInlineSiteMap;

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CModuleInfo);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CModuleInfo);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IModuleInfo3*>(this),
                static_cast<IModuleInfo2*>(this),
                static_cast<IModuleInfo*>(this),
                static_cast<IDataContainer*>(static_cast<CDataContainer*>(this)),
                riid,
                ppvObject
            );
        }

    public:
        CModuleInfo();
        ~CModuleInfo();

        HRESULT Initialize(
            _In_ ModuleID moduleID,
            _In_ WCHAR* wszModulePath,
            _In_ IAssemblyInfo* pAssemblyInfo,
            _In_ IAppDomainInfo* pAppDomainInfo,
            _In_ LPCBYTE pModuleBaseLoadAddress,
            _In_ IMetaDataImport2* pMetadataImport,
            _In_ IMetaDataAssemblyImport* pMetadataAssemblyImport,
            _In_ IMetaDataEmit2* pMetaDataEmit2,
            _In_ IMetaDataAssemblyEmit* pMetaDataAssemblyEmit
            );

        HRESULT Dispose();

    public:
        HRESULT GetMethodInfoById(_In_ FunctionID functionId, CMethodInfo** ppMethodInfo);
        HRESULT GetMethodInfoByToken(_In_ mdToken methodToken, CMethodInfo** ppMethodInfo);

        HRESULT AddMethodInfo(_In_ FunctionID functionId, CMethodInfo* pMethodInfo);
        HRESULT ReleaseMethodInfo(_In_ FunctionID functionId);

        // The profiler apis have issues around rejit request revert and generics
        // If a function is modified by the profiler, GetILFunctionBody may return
        // the modified IL (example: rejit before original jit).
        // So, if any consumer requests the il function body, the original il is cached
        // and resused in requests that come later.
        //
        // NOTE: this is unfortunate as this is yet another copy of the il.
        HRESULT GetMethodIl(
            _In_ ICorProfilerInfo* pCorProfilerInfo,
            _In_ mdToken tkFunction,
            _Out_ IMAGE_COR_ILMETHOD** ppMethodHeader,
            _Out_ ULONG* pcbMethodSize
            );

        HRESULT IncrementMethodRejitCount(_In_ mdToken token);
        HRESULT GetMethodRejitCount(_In_ mdToken token, _Out_ DWORD* pdwRejitCount);

        HRESULT SetRejitMethodInfo(_In_ mdToken token, _In_opt_ CMethodInfo* pMethodInfo);

        HRESULT GetInlineSiteMap(_Out_ CInlineSiteMap** ppInilineSiteMap);

        // Sets the instrumentation map for normal, non rejit scenarios.
        void SetILInstrumentationMap(_In_ CMethodInfo* pMethodInfo, _In_ CSharedArray<COR_IL_MAP> map);

        HRESULT GetILInstrumentationMap(_In_ CMethodJitInfo* pMethodJitInfo, _In_ ULONG32 cMap, _Out_writes_(cMap) COR_IL_MAP* pMap, _Out_ ULONG32* pcNeeded);
        void SetMethodIsTransformed(_In_ mdMethodDef methodDef, bool isInstrumented);
        bool GetIsMethodInstrumented(_In_ mdMethodDef methodDef);

        // IModuleInfo methods
    public:
        virtual HRESULT __stdcall GetModuleName(_Out_ BSTR* pbstrModuleName) override;
        virtual HRESULT __stdcall GetFullPath(_Out_ BSTR* pbstrFullPath) override;

        // Get info about the assembly that contains this module
        virtual HRESULT __stdcall GetAssemblyInfo(_Out_ IAssemblyInfo** pAssemblyInfo) override;

        virtual HRESULT __stdcall GetAppDomainInfo(_Out_ IAppDomainInfo** ppAppDomainInfo) override;

        virtual HRESULT __stdcall GetMetaDataImport(_Out_ IUnknown** ppMetaDataImport) override;
        virtual HRESULT __stdcall GetMetaDataAssemblyImport(_Out_ IUnknown** ppMetaDataAssemblyImport) override;
        virtual HRESULT __stdcall GetMetaDataEmit(_Out_ IUnknown** ppMetaDataEmit) override;
        virtual HRESULT __stdcall GetMetaDataAssemblyEmit(_Out_ IUnknown** ppMetaDataAssemblyEmit) override;

        virtual HRESULT __stdcall GetModuleID(_Out_ ModuleID* pModuleId) override;

        // Get the managed module version id
        virtual HRESULT __stdcall GetMVID(_Out_ GUID* pguidMvid) override;

        virtual HRESULT __stdcall GetIsILOnly(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsMscorlib(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsDynamic(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsNgen(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsWinRT(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIs64bit(_Out_ BOOL* pbValue) override;

        virtual HRESULT __stdcall GetImageBase(_Out_ LPCBYTE* pbValue) override;

        virtual HRESULT __stdcall  GetCorHeader(_In_ DWORD cbValue, _Out_writes_(cbValue) BYTE* pbValue) override;

        // Returns the modules entrypoint token. If the module has no entrypoint token, returns E_FAIL;
        virtual HRESULT __stdcall GetEntrypointToken(_Out_ DWORD* pdwEntrypointToken) override;

        // Returns the VS_FIXEDFILEINFO for this module.
        virtual HRESULT __stdcall GetModuleVersion(_In_ DWORD cbValue, _Out_writes_(cbValue) BYTE* pbValue) override;

        // Request a rejit on a method
        virtual HRESULT __stdcall RequestRejit(_In_ mdToken methodToken) override;

        virtual HRESULT __stdcall CreateTypeFactory(_Out_ ITypeCreator** ppTypeFactory) override;

        // Allow instrumentation methods to receive an instance of IMethodInfo for a FunctionID.
        // This method info is only for obtaining information about the method and cannot be used
        // for instrumenation.
        virtual HRESULT __stdcall GetMethodInfoById(_In_ FunctionID functionID, _Out_ IMethodInfo** ppMethodInfo) override;

        // Allow instrumentation methods to receive an instance of IMethodInfo for a token.
        // This method info is only for obtaining information about the method and cannot be used
        // for instrumenation.
        virtual HRESULT __stdcall GetMethodInfoByToken(_In_ mdToken token, _Out_ IMethodInfo** ppMethodInfo) override;

        // Import source module into the current one
        virtual HRESULT __stdcall ImportModule(_In_ IUnknown* pSourceModule, _In_ LPCBYTE* pSourceImage) override;

        // IModuleInfo2 methods
    public:
        virtual HRESULT __stdcall GetIsFlatLayout(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall ResolveRva(_In_ DWORD rva, _Out_ LPCBYTE* ppbResolvedAddress) override;

        // IModuleInfo3
    public:
        virtual HRESULT __stdcall GetIsLoadedFromDisk(_Out_ BOOL* pbValue) override;


    private:
        HRESULT GetModuleTypeFlags();
        HRESULT ReadModuleHeaders();
        HRESULT DetermineIfIsMscorlib();
        HRESULT ReadFixedFileVersion();

        HRESULT AddInlineSitesForRejit(_In_ mdToken methodToken, _In_ vector<ModuleID>& moduleIds, _In_ vector<mdToken>& methodTokens);

        HRESULT ResolveRva(_In_ LPCBYTE pbSectionStart, _In_ WORD numSections, _In_ DWORD rva, _Out_ LPCBYTE* ppbResolvedAddress);
    };
}
