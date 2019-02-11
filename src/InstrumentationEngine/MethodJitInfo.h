// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

namespace MicrosoftInstrumentationEngine
{
    class CProfilerManager;

    class __declspec(uuid("{AC76DC45-B4EA-451D-9851-66DA2338D6F2}"))
        CMethodJitInfo : public IMethodJitInfo2, public CModuleRefCount
    {
    friend class CProfilerManager;

    private:
        FunctionID m_functionId;
        HRESULT m_hresult;
        BOOL m_isRejit;
        ReJITID m_rejitId;
        CComPtr<CProfilerManager> m_pProfilerManager;
        
        BOOL m_isTransformed = false;
        CComPtr<CModuleInfo> m_pModuleInfo;
        mdMethodDef m_methodToken = mdTokenNil;
        HRESULT m_initializeResult = S_OK;
        UINT_PTR m_codeAddress = 0;
        bool m_isAddressQueried = false;
        

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CMethodJitInfo);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CMethodJitInfo);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IMethodJitInfo*>(this),
                static_cast<IMethodJitInfo2*>(this),
                static_cast<IUnknown*>(this),
                riid,
                ppvObject
            );
        }

    public:
        CMethodJitInfo(FunctionID functionId, HRESULT hresult, BOOL isRejit, ReJITID rejitId, CProfilerManager* pProfilerManager) : 
            m_functionId(functionId),
            m_hresult(hresult), 
            m_isRejit(isRejit), 
            m_rejitId(rejitId),
            m_pProfilerManager(pProfilerManager) {}
        virtual ~CMethodJitInfo() = default;

    public:
        // IMethodJitInfo
        STDMETHOD(GetFunctionID)(_Out_ FunctionID* pFunctionId) override;
        STDMETHOD(GetIsRejit)(_Out_ BOOL* pIsRejit) override;
        STDMETHOD(GetJitHR)(_Out_ HRESULT* pHr) override;
        STDMETHOD(GetILTransformationStatus)(_Out_ BOOL* pIsTransformed) override;
        STDMETHOD(GetRejitId)(_Out_ ReJITID* pRejitId) override;
        STDMETHOD(GetModuleInfo)(_Out_ IModuleInfo** ppModuleInfo) override;

        // IMethodJitInfo2
        STDMETHOD(GetILNativeMapping)(_In_ ULONG32 cMap, _Out_writes_(cMap) COR_DEBUG_IL_TO_NATIVE_MAP* pMap, _Out_ ULONG32* pcNeeded) override;
        STDMETHOD(GetILInstrumentationMap)(_In_ ULONG32 cMap, _Out_writes_(cMap) COR_IL_MAP* pMap, _Out_ ULONG32* pcNeeded) override;
        STDMETHOD(GetMethodToken)(_Out_ mdMethodDef* pMethodDef) override;
        STDMETHOD(GetNativeCodeAddress)(_Out_ UINT_PTR *pCodeAddress) override;
    private:
        HRESULT EnsureInitialized();
    };
}