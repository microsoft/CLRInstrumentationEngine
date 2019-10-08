// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ProfilerManager.h"

namespace MicrosoftInstrumentationEngine
{

    class CProfilerManagerWrapper :
        public IProfilerManager,
        public IProfilerManager2,
        public IProfilerManager3,
        public IProfilerManager4,
        public IProfilerManager5,
        public IProfilerManagerLogging,
        public CModuleRefCount
    {
    public:
        CProfilerManagerWrapper(GUID instrumentationMethodGuid, CProfilerManager* pProfilerManager);

    private:
        static const int LogEntryMaxSize = 4096;
        static LPCWSTR wszLogFormattablePrefix;
        tstring m_wszInstrumentationMethodGuid;
        CComPtr<CProfilerManager> m_pProfilerManager;

        // IUnknown Members
    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CProfilerManagerWrapper);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CProfilerManagerWrapper);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
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
        STDMETHOD(SetupProfilingEnvironment)(_In_reads_(numConfigPaths) BSTR bstrConfigPaths[], _In_ UINT numConfigPaths);

        STDMETHOD(AddRawProfilerHook)(
            _In_ IUnknown *pUnkProfilerCallback
            );

        STDMETHOD(RemoveRawProfilerHook)(
            );

        STDMETHOD(GetCorProfilerInfo)(
            _Outptr_ IUnknown **ppCorProfiler
            );

        STDMETHOD(GetRuntimeType)(_Out_ COR_PRF_RUNTIME_TYPE* pRuntimeType);

        STDMETHOD(GetProfilerHost)(_Out_ IProfilerManagerHost** ppProfilerManagerHost);

        STDMETHOD(GetLoggingInstance)(_Out_ IProfilerManagerLogging** ppLogging);

        STDMETHOD(SetLoggingHost)(_In_opt_ IProfilerManagerLoggingHost* pLoggingHost);

        STDMETHOD(GetAppDomainCollection)(_Out_ IAppDomainCollection** ppAppDomainCollection);

        STDMETHOD(CreateSignatureBuilder)(_Out_ ISignatureBuilder ** ppSignatureBuilder);

        STDMETHOD(GetInstrumentationMethod)(_In_ REFGUID cslid, _Out_ IUnknown** ppUnknown);

        STDMETHOD(RemoveInstrumentationMethod(_In_ IInstrumentationMethod* pInstrumentationMethod));

        STDMETHOD(AddInstrumentationMethod)(_In_ BSTR bstrModulePath, _In_ BSTR bstrName, _In_ BSTR bstrDescription, _In_ BSTR bstrModule, _In_ BSTR bstrClassGuid, _In_ DWORD dwPriority, _Out_ IInstrumentationMethod** ppInstrumentationMethod);

        // IProfilerManager2 Methods
    public:
        STDMETHOD(DisableProfiling)();

        STDMETHOD(ApplyMetadata)(_In_ IModuleInfo* pMethodInfo);

        // IProfilerManager3 Methods
    public:
        STDMETHOD(GetApiVersion)(_Out_ DWORD* pApiVer);

        // IProfilerManager4 Methods
    public:
        STDMETHOD(GetGlobalLoggingInstance)(_Out_ IProfilerManagerLogging** ppLogging);

        // IProfilerManager5 Methods
    public:
        STDMETHOD(IsInstrumentationMethodRegistered)(_In_ REFGUID clsid, _Out_ BOOL* pfRegistered);

        // IProfilerManagerLogging Methods
    public:
        STDMETHOD(LogMessage)(_In_ const WCHAR* wszMessage);

        STDMETHOD(LogError)(_In_ const WCHAR* wszError);

        STDMETHOD(LogDumpMessage)(_In_ const WCHAR* wszMessage);

        STDMETHOD(EnableDiagnosticLogToDebugPort)(_In_ BOOL enable);

        STDMETHOD(GetLoggingFlags)(_Out_ LoggingFlags* pLoggingFlags);

        STDMETHOD(SetLoggingFlags)(_In_ LoggingFlags loggingFlags);
    };
}