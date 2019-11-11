// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ProfilerManager.h"

namespace MicrosoftInstrumentationEngine
{
    class CProfilerManagerForInstrumentationMethod :
        public IProfilerManagerContract,
        public CModuleRefCount
    {
    public:
        CProfilerManagerForInstrumentationMethod(GUID instrumentationMethodGuid, CProfilerManager* pProfilerManager);

    private:
        static const int LogEntryMaxSize = 4096;
        static LPCWSTR wszLogFormattablePrefix;
        GUID m_instrumentationMethodGuid;
        tstring m_wszInstrumentationMethodGuid;
        CComPtr<CProfilerManager> m_pProfilerManager;
        LoggingFlags m_flags;
        bool m_bDisableMethodPrefix;

        // IUnknown Members
    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CProfilerManagerForInstrumentationMethod);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CProfilerManagerForInstrumentationMethod);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            // TODO: pull these interfaces from IProfilerManagerContract instead.
            return ImplQueryInterface(
                static_cast<IProfilerManager*>(this),
                static_cast<IProfilerManager2*>(this),
                static_cast<IProfilerManager3*>(this),
                static_cast<IProfilerManager4*>(this),
                static_cast<IProfilerManager5*>(this),
                static_cast<IProfilerManagerLogging*>(this),
                riid,
                ppvObject
            );
        }
        // IProfilerManager methods
    public:
        STDMETHOD(SetupProfilingEnvironment)(_In_reads_(numConfigPaths) BSTR bstrConfigPaths[], _In_ UINT numConfigPaths) override;

        STDMETHOD(AddRawProfilerHook)(
            _In_ IUnknown *pUnkProfilerCallback
            ) override;

        STDMETHOD(RemoveRawProfilerHook)(
            ) override;

        STDMETHOD(GetCorProfilerInfo)(
            _Outptr_ IUnknown **ppCorProfiler
            ) override;

        STDMETHOD(GetRuntimeType)(_Out_ COR_PRF_RUNTIME_TYPE* pRuntimeType) override;

        STDMETHOD(GetProfilerHost)(_Out_ IProfilerManagerHost** ppProfilerManagerHost) override;

        STDMETHOD(GetLoggingInstance)(_Out_ IProfilerManagerLogging** ppLogging) override;

        STDMETHOD(SetLoggingHost)(_In_opt_ IProfilerManagerLoggingHost* pLoggingHost) override;

        STDMETHOD(GetAppDomainCollection)(_Out_ IAppDomainCollection** ppAppDomainCollection) override;

        STDMETHOD(CreateSignatureBuilder)(_Out_ ISignatureBuilder ** ppSignatureBuilder) override;

        STDMETHOD(GetInstrumentationMethod)(_In_ REFGUID cslid, _Out_ IUnknown** ppUnknown) override;

        STDMETHOD(RemoveInstrumentationMethod(_In_ IInstrumentationMethod* pInstrumentationMethod)) override;

        STDMETHOD(AddInstrumentationMethod)(_In_ BSTR bstrModulePath, _In_ BSTR bstrName, _In_ BSTR bstrDescription, _In_ BSTR bstrModule, _In_ BSTR bstrClassGuid, _In_ DWORD dwPriority, _Out_ IInstrumentationMethod** ppInstrumentationMethod) override;

        // IProfilerManager2 Methods
    public:
        STDMETHOD(DisableProfiling)() override;

        STDMETHOD(ApplyMetadata)(_In_ IModuleInfo* pMethodInfo) override;

        // IProfilerManager3 Methods
    public:
        STDMETHOD(GetApiVersion)(_Out_ DWORD* pApiVer) override;

        // IProfilerManager4 Methods
    public:
        STDMETHOD(GetGlobalLoggingInstance)(_Out_ IProfilerManagerLogging** ppLogging) override;

        // IProfilerManager5 Methods
    public:
        STDMETHOD(IsInstrumentationMethodRegistered)(_In_ REFGUID clsid, _Out_ BOOL* pfRegistered) override;

        // IProfilerManagerLogging Methods
    public:
        STDMETHOD(LogMessage)(_In_ const WCHAR* wszMessage) override;

        STDMETHOD(LogError)(_In_ const WCHAR* wszError) override;

        STDMETHOD(LogDumpMessage)(_In_ const WCHAR* wszMessage) override;

        STDMETHOD(EnableDiagnosticLogToDebugPort)(_In_ BOOL enable) override;

        STDMETHOD(GetLoggingFlags)(_Out_ LoggingFlags* pLoggingFlags) override;

        STDMETHOD(SetLoggingFlags)(_In_ LoggingFlags loggingFlags) override;

    public:
        STDMETHOD(GetInstruMethodLoggingFlags)(_Out_ LoggingFlags* pLoggingFlags);

    private:
        STDMETHOD(LogMessageInternal)(_In_ const WCHAR* wszMessage, _In_ LoggingFlags flagToCheck);
    };

    void EscapeFormatSpecifiers(_In_ const wstring tsOriginal, _Inout_ wstring& tsEscaped);
}