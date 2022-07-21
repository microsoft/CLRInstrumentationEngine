// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"
#include "../../../../InstrumentationEngine.Api/InstrumentationEngine.h"

class __declspec(uuid("D2959618-F9B6-4CB6-80CF-F3B0E3263777"))
    CInstanaInstrumentationMethod :
    public IInstrumentationMethod
{
private:
    static const WCHAR TestOutputPathEnvName[];
    static const WCHAR TestScriptFileEnvName[];
    static const WCHAR TestScriptFolder[];
    static const WCHAR IsRejitEnvName[];

    unordered_map<std::wstring, CorElementType> m_typeMap;

    //vector<shared_ptr<CInstrumentMethodEntry>> m_instrumentMethodEntries;

    //unordered_map<IMethodInfo*, shared_ptr<CInstrumentMethodEntry>> m_methodInfoToEntryMap;

    bool m_bExceptionTrackingEnabled;
    bool m_bTestInstrumentationMethodLogging;

    //shared_ptr<CInjectAssembly> m_spInjectAssembly;

    std::wstring m_strBinaryDir;

    CComPtr<IProfilerManager> m_pProfilerManager;
    CComPtr<IProfilerStringManager> m_pStringManager;

public:
    CInstanaInstrumentationMethod() : m_typeMap
    ({
        { L"System.Boolean", ELEMENT_TYPE_BOOLEAN },
        { L"System.Char", ELEMENT_TYPE_CHAR },

        { L"System.SByte", ELEMENT_TYPE_I1 },
        { L"System.Int16", ELEMENT_TYPE_I2 },
        { L"System.Int32", ELEMENT_TYPE_I4 },
        { L"System.Int64", ELEMENT_TYPE_I8 },

        { L"System.Byte", ELEMENT_TYPE_U1 },
        { L"System.UInt16", ELEMENT_TYPE_U2 },
        { L"System.Unt32", ELEMENT_TYPE_U4 },
        { L"System.Unt64", ELEMENT_TYPE_U8 },

        { L"System.IntPtr", ELEMENT_TYPE_I },
        { L"System.UIntPtr", ELEMENT_TYPE_U },

        { L"System.Single", ELEMENT_TYPE_R4 },
        { L"System.Double", ELEMENT_TYPE_R8 },

        { L"System.String", ELEMENT_TYPE_STRING },
        { L"System.Object", ELEMENT_TYPE_OBJECT },
        }),
        m_bExceptionTrackingEnabled(false),
        m_bTestInstrumentationMethodLogging(false),
        m_spInjectAssembly(nullptr)
    {
    }


    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }

    static HRESULT STDMETHODCALLTYPE StackSnapshotCallbackExceptionThrown(
        _In_ FunctionID funcId,
        _In_ UINT_PTR ip,
        _In_ COR_PRF_FRAME_INFO frameInfo,
        _In_ ULONG32 contextSize,
        _In_reads_(contextSize) BYTE context[],
        _In_ void* clientData
    );

    // IInstrumentationMethod
public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(_In_ IProfilerManager* pProfilerManager);

    virtual HRESULT STDMETHODCALLTYPE OnAppDomainCreated(
        _In_ IAppDomainInfo* pAppDomainInfo);

    virtual HRESULT STDMETHODCALLTYPE OnAppDomainShutdown(
        _In_ IAppDomainInfo* pAppDomainInfo);

    virtual HRESULT STDMETHODCALLTYPE OnAssemblyLoaded(_In_ IAssemblyInfo* pAssemblyInfo);
    virtual HRESULT STDMETHODCALLTYPE OnAssemblyUnloaded(_In_ IAssemblyInfo* pAssemblyInfo);

    virtual HRESULT STDMETHODCALLTYPE OnModuleLoaded(_In_ IModuleInfo* pModuleInfo);
    virtual HRESULT STDMETHODCALLTYPE OnModuleUnloaded(_In_ IModuleInfo* pModuleInfo);

    virtual HRESULT STDMETHODCALLTYPE OnShutdown();

    virtual HRESULT STDMETHODCALLTYPE ShouldInstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit, _Out_ BOOL* pbInstrument);

    virtual HRESULT STDMETHODCALLTYPE BeforeInstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit);

    virtual HRESULT STDMETHODCALLTYPE InstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit);

    virtual HRESULT STDMETHODCALLTYPE OnInstrumentationComplete(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit) { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE AllowInlineSite(_In_ IMethodInfo* pMethodInfoInlinee, _In_ IMethodInfo* pMethodInfoCaller, _Out_ BOOL* pbAllowInline);


};
