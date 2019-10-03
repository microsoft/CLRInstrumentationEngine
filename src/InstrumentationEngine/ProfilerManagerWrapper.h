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
        ~CProfilerManagerWrapper();

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

        // return the profiler host instance
        STDMETHOD(GetProfilerHost)(_Out_ IProfilerManagerHost** ppProfilerManagerHost);

        // Returns an instance of IProfilerManagerLogging which instrumentation methods can use
        // to log to the profiler manager or optionally to the profiler host.
        STDMETHOD(GetLoggingInstance)(_Out_ IProfilerManagerLogging** ppLogging);

        // By default, logging messages are written to the debug output port. However,
        // hosts can optionally signup to receive them through an instance of IProfilerManagerLoggingHost
        STDMETHOD(SetLoggingHost)(_In_opt_ IProfilerManagerLoggingHost* pLoggingHost);

        STDMETHOD(GetAppDomainCollection)(_Out_ IAppDomainCollection** ppAppDomainCollection);

        STDMETHOD(CreateSignatureBuilder)(_Out_ ISignatureBuilder ** ppSignatureBuilder);

        // Query the profiler manager for a pointer to another instrumentation method.
        // Consumers can call QueryInterface to obtain an interface specific to that
        // instrumentation method
        STDMETHOD(GetInstrumentationMethod)(_In_ REFGUID cslid, _Out_ IUnknown** ppUnknown);

        STDMETHOD(RemoveInstrumentationMethod(_In_ IInstrumentationMethod* pInstrumentationMethod));

        // Registers a new instrumentation method in the profiler manager. Also calls its Initialize() method.
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
        // If trace logging in enabled in the profiler manager, trace messages are sent to the
        // profiler manager through this function.
        STDMETHOD(LogMessage)(_In_ const WCHAR* wszMessage);

        // Errors detected during profiling will be sent to the host through this method
        STDMETHOD(LogError)(_In_ const WCHAR* wszError);

        // If instrumentation result tracing is enabled, the detailed results of each instrumented
        // method will be sent to the profiler manager host through this method.
        STDMETHOD(LogDumpMessage)(_In_ const WCHAR* wszMessage);

        // Called to cause logging to be written to the debug output port (via DebugOutputString) as well
        // as to the host.
        STDMETHOD(EnableDiagnosticLogToDebugPort)(_In_ BOOL enable);

        // Allows instrumentation methods and hosts to ask for the current logging flags
        STDMETHOD(GetLoggingFlags)(_Out_ LoggingFlags* pLoggingFlags);

        // Allows instrumentation methods and hosts to modify the current logging flags
        STDMETHOD(SetLoggingFlags)(_In_ LoggingFlags loggingFlags);

    private:
        tstring m_wszInstrumentationMethodGuid;
        CComPtr<CProfilerManager> m_pProfilerManager;
    };
}