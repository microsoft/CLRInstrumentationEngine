// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "NaglerInstrumentMethodEntry.h"

class __declspec(uuid("D2959618-F9B6-4CB6-80CF-F3B0E3263888"))
CInstrumentationMethod :
    public IInstrumentationMethod,
    public IInstrumentationMethodExceptionEvents,
    public CComObjectRootEx<CComMultiThreadModel>,
    public CComCoClass<CInstrumentationMethod, &__uuidof(CInstrumentationMethod)>
{
private:
    static const WCHAR TestOutputPathEnvName[];
    static const WCHAR TestScriptFileEnvName[];
    static const WCHAR TestScriptFolder[];
    static const WCHAR IsRejitEnvName[];

    unordered_map<std::wstring, CorElementType> m_typeMap;

    vector<shared_ptr<CInstrumentMethodEntry>> m_instrumentMethodEntries;

    unordered_map<IMethodInfo*, shared_ptr<CInstrumentMethodEntry>> m_methodInfoToEntryMap;

    bool m_bExceptionTrackingEnabled;

    shared_ptr<CInjectAssembly> m_spInjectAssembly;

    std::wstring m_strBinaryDir;

    CComPtr<IProfilerManager> m_pProfilerManager;
    CComPtr<IProfilerManagerLogging> m_pProfilerManagerLogger;

public:
    CInstrumentationMethod() : m_typeMap
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
    m_spInjectAssembly(nullptr)
    {
    }

    BEGIN_COM_MAP(CInstrumentationMethod)
        COM_INTERFACE_ENTRY(IInstrumentationMethod)
        COM_INTERFACE_ENTRY(IInstrumentationMethodExceptionEvents)
    END_COM_MAP()

    DECLARE_REGISTRY_RESOURCEID(IDR_CINSTRMETHOD)

    // Ensure only a single instance of CInstrumentationMethod is created
    DECLARE_CLASSFACTORY_SINGLETON(CInstrumentationMethod)

    DECLARE_PROTECT_FINAL_CONSTRUCT()

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
        _In_ void *clientData
        );

    // IInstrumentationMethod
public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(_In_ IProfilerManager* pProfilerManager);

    virtual HRESULT STDMETHODCALLTYPE OnAppDomainCreated(
        _In_ IAppDomainInfo *pAppDomainInfo);

    virtual HRESULT STDMETHODCALLTYPE OnAppDomainShutdown(
        _In_ IAppDomainInfo *pAppDomainInfo);

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

public:
    virtual HRESULT STDMETHODCALLTYPE ExceptionCatcherEnter(
        _In_ IMethodInfo* pMethodInfo,
        _In_ UINT_PTR   objectId
        );

    virtual HRESULT STDMETHODCALLTYPE ExceptionCatcherLeave();

    virtual HRESULT STDMETHODCALLTYPE ExceptionSearchCatcherFound(
        _In_ IMethodInfo* pMethodInfo
        );

    virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFilterEnter(
        _In_ IMethodInfo* pMethodInfo
        );

    virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFilterLeave();

    virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionEnter(
        _In_ IMethodInfo* pMethodInfo
        );

    virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionLeave();

    virtual HRESULT STDMETHODCALLTYPE ExceptionThrown(
        _In_ UINT_PTR thrownObjectId
        );

    virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyEnter(
        _In_ IMethodInfo* pMethodInfo
        );

    virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyLeave();

    virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionEnter(
        _In_ IMethodInfo* pMethodInfo
        );

    virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionLeave();

private:
    HRESULT LoadTestScript();
    HRESULT ProcessInstrumentMethodNode(IXMLDOMNode* pNode);
    HRESULT ProcessInstructionNodes(IXMLDOMNode* pNode, vector<shared_ptr<CInstrumentInstruction>>& instructions, bool isBaseline);
    HRESULT ProcessLocals(IXMLDOMNode* pNode, vector<CLocalType>& locals);
    HRESULT ProcessPointTo(IXMLDOMNode* pNode, CInstrumentMethodPointTo& pointTo);
    HRESULT ProcessInjectAssembly(IXMLDOMNode* pNode, shared_ptr<CInjectAssembly>& injectAssembly);

    HRESULT InstrumentLocals(IMethodInfo* pMethodInfo, shared_ptr<CInstrumentMethodEntry> pMethodEntry);
    HRESULT GetType(IModuleInfo* moduleInfo, const CLocalType &localType, IType** pType);

    HRESULT ConvertOpcode(BSTR bstrOpcode, ILOrdinalOpcode* pOpcode, ILOpcodeInfo* pOpcodeInfo);

    static DWORD WINAPI InstrumentationMethodThreadProc(
        _In_  LPVOID lpParameter
        );

    HRESULT HandleStackSnapshotCallbackExceptionThrown(
        _In_ FunctionID funcId,
        _In_ UINT_PTR ip,
        _In_ COR_PRF_FRAME_INFO frameInfo,
        _In_ ULONG32 contextSize,
        _In_reads_(contextSize) BYTE context[]
        );
};

#ifndef PLATFORM_UNIX
OBJECT_ENTRY_AUTO(__uuidof(CInstrumentationMethod), CInstrumentationMethod)
#endif
