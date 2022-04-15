// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"
#include "NaglerInstrumentMethodEntry.h"

class __declspec(uuid("D2959618-F9B6-4CB6-80CF-F3B0E3263888"))
CInstrumentationMethod :
    public IInstrumentationMethod,
    public IInstrumentationMethodExceptionEvents,
    public CModuleRefCount
{
private:
    static const WCHAR TestOutputPathEnvName[];
    static const WCHAR TestScriptFileEnvName[];
    static const WCHAR TestScriptFolder[];
    static const WCHAR IsRejitEnvName[];

    unordered_map<tstring, CorElementType> m_typeMap;

    vector<shared_ptr<CInstrumentMethodEntry>> m_instrumentMethodEntries;

    unordered_map<IMethodInfo*, shared_ptr<CInstrumentMethodEntry>> m_methodInfoToEntryMap;

    bool m_bExceptionTrackingEnabled;
    bool m_bTestInstrumentationMethodLogging;

    shared_ptr<CInjectAssembly> m_spInjectAssembly;

    tstring m_strBinaryDir;

    CComPtr<IProfilerManager> m_pProfilerManager;
    CComPtr<IProfilerStringManager> m_pStringManager;

    // CModuleRefCount
public:
    DEFINE_DELEGATED_REFCOUNT_ADDREF(CInstrumentationMethod);
    DEFINE_DELEGATED_REFCOUNT_RELEASE(CInstrumentationMethod);
    STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void** ppvObject) override
    {
        return ImplQueryInterface(
            static_cast<IInstrumentationMethod*>(this),
            static_cast<IInstrumentationMethodExceptionEvents*>(this),
            riid,
            ppvObject
        );
    }

public:
    CInstrumentationMethod() : m_typeMap
    ({
        { _T("System.Boolean"), ELEMENT_TYPE_BOOLEAN },
        { _T("System.Char"), ELEMENT_TYPE_CHAR },

        { _T("System.SByte"), ELEMENT_TYPE_I1 },
        { _T("System.Int16"), ELEMENT_TYPE_I2 },
        { _T("System.Int32"), ELEMENT_TYPE_I4 },
        { _T("System.Int64"), ELEMENT_TYPE_I8 },

        { _T("System.Byte"), ELEMENT_TYPE_U1 },
        { _T("System.UInt16"), ELEMENT_TYPE_U2 },
        { _T("System.Unt32"), ELEMENT_TYPE_U4 },
        { _T("System.Unt64"), ELEMENT_TYPE_U8 },

        { _T("System.IntPtr"), ELEMENT_TYPE_I },
        { _T("System.UIntPtr"), ELEMENT_TYPE_U },

        { _T("System.Single"), ELEMENT_TYPE_R4 },
        { _T("System.Double"), ELEMENT_TYPE_R8 },

        { _T("System.String"), ELEMENT_TYPE_STRING },
        { _T("System.Object"), ELEMENT_TYPE_OBJECT },
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
        _In_ void *clientData
        );

    // IInstrumentationMethod
public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(_In_ IProfilerManager* pProfilerManager) override;

    virtual HRESULT STDMETHODCALLTYPE OnAppDomainCreated(
        _In_ IAppDomainInfo *pAppDomainInfo) override;

    virtual HRESULT STDMETHODCALLTYPE OnAppDomainShutdown(
        _In_ IAppDomainInfo *pAppDomainInfo) override;

    virtual HRESULT STDMETHODCALLTYPE OnAssemblyLoaded(_In_ IAssemblyInfo* pAssemblyInfo) override;
    virtual HRESULT STDMETHODCALLTYPE OnAssemblyUnloaded(_In_ IAssemblyInfo* pAssemblyInfo) override;

    virtual HRESULT STDMETHODCALLTYPE OnModuleLoaded(_In_ IModuleInfo* pModuleInfo) override;
    virtual HRESULT STDMETHODCALLTYPE OnModuleUnloaded(_In_ IModuleInfo* pModuleInfo) override;

    virtual HRESULT STDMETHODCALLTYPE OnShutdown() override;

    virtual HRESULT STDMETHODCALLTYPE ShouldInstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit, _Out_ BOOL* pbInstrument) override;

    virtual HRESULT STDMETHODCALLTYPE BeforeInstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit) override;

    virtual HRESULT STDMETHODCALLTYPE InstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit) override;

    virtual HRESULT STDMETHODCALLTYPE OnInstrumentationComplete(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit) override { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE AllowInlineSite(_In_ IMethodInfo* pMethodInfoInlinee, _In_ IMethodInfo* pMethodInfoCaller, _Out_ BOOL* pbAllowInline) override;

public:
    virtual HRESULT STDMETHODCALLTYPE ExceptionCatcherEnter(
        _In_ IMethodInfo* pMethodInfo,
        _In_ UINT_PTR   objectId
        ) override;

    virtual HRESULT STDMETHODCALLTYPE ExceptionCatcherLeave() override;

    virtual HRESULT STDMETHODCALLTYPE ExceptionSearchCatcherFound(
        _In_ IMethodInfo* pMethodInfo
        ) override;

    virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFilterEnter(
        _In_ IMethodInfo* pMethodInfo
        ) override;

    virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFilterLeave() override;

    virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionEnter(
        _In_ IMethodInfo* pMethodInfo
        ) override;

    virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionLeave() override;

    virtual HRESULT STDMETHODCALLTYPE ExceptionThrown(
        _In_ UINT_PTR thrownObjectId
        ) override;

    virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyEnter(
        _In_ IMethodInfo* pMethodInfo
        ) override;

    virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyLeave() override;

    virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionEnter(
        _In_ IMethodInfo* pMethodInfo
        ) override;

    virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionLeave() override;

private:
    HRESULT LoadTestScript();
    HRESULT ProcessInstrumentMethodNode(CXmlNode* pNode);
    HRESULT ProcessInstructionNodes(CXmlNode* pNode, vector<shared_ptr<CInstrumentInstruction>>& instructions, bool isBaseline);
    HRESULT ProcessLocals(CXmlNode* pNode, vector<CLocalType>& locals);
    HRESULT ProcessPointTo(CXmlNode* pNode, CInstrumentMethodPointTo& pointTo);
    HRESULT ProcessInjectAssembly(CXmlNode* pNode, shared_ptr<CInjectAssembly>& injectAssembly);

    HRESULT InstrumentLocals(IMethodInfo* pMethodInfo, shared_ptr<CInstrumentMethodEntry> pMethodEntry);
    HRESULT GetType(IModuleInfo* moduleInfo, const CLocalType &localType, IType** pType);

    HRESULT ConvertOpcode(LPCWSTR zwOpcode, ILOrdinalOpcode* pOpcode, ILOpcodeInfo* pOpcodeInfo);
    HRESULT AddExceptionHandler(IMethodInfo* pMethodInfo, IInstructionGraph* pInstructionGraph);
    HRESULT PerformSingleReturnInstrumentation(IMethodInfo* pMethodInfo, IInstructionGraph* pInstructionGraph);

    static DWORD WINAPI LoadInstrumentationMethodXml(
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
