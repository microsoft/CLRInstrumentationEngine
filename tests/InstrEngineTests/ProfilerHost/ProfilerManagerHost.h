// Copyright (c) Microsoft Corporation. All rights reserved.
// 

// ProfilerManagerHost.h : Declaration of the CProfilerManagerHost

#pragma once

class CProfilerManagerHost;

class __declspec(uuid("6AB2072F-6241-49A6-86FF-05E98A9E8748"))
CProfilerManagerHost :
    public IProfilerManagerHost,
    public IProfilerManagerLoggingHost,
    public CComObjectRootEx<CComMultiThreadModel>,
    public CComCoClass<CProfilerManagerHost, &__uuidof(CProfilerManagerHost)>
{
private:
    static const WCHAR HostConfigPathEnvName[];
    static const WCHAR TestOutputFileEnvName[];
    static const WCHAR TestResultFolder[];

    CHandle m_hOutputFile;

    static CProfilerManagerHost* s_instance;

public:
    CProfilerManagerHost()
    {
        s_instance = this;
    }

    static CProfilerManagerHost* GetInstance()
    {
        return s_instance;
    }

BEGIN_COM_MAP(CProfilerManagerHost)
    COM_INTERFACE_ENTRY(IProfilerManagerHost)
    COM_INTERFACE_ENTRY(IProfilerManagerLoggingHost)
END_COM_MAP()

DECLARE_REGISTRY_RESOURCEID(IDR_CPROFILERHOST)

// Ensure only a single instance of CProfilerManagerHost is created
DECLARE_CLASSFACTORY_SINGLETON(CProfilerManagerHost)

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }

    // IProfilerManagerHost
public:
    HRESULT STDMETHODCALLTYPE Initialize(_In_ IProfilerManager* pProfilerManager);

    // IProfilerManagerLoggingHost
public:
    // If trace logging in enabled in the profiler manager, all trace messages are sent to the
    // profiler manager through this function.
    HRESULT STDMETHODCALLTYPE LogMessage(_In_ const WCHAR* wszMessage);

    // Errors detected during profiling will be sent to the host through this method
    HRESULT STDMETHODCALLTYPE LogError(_In_ const WCHAR* wszError);

    // If instrumentation result tracing is enabled, the detailed results of each instrumented
    // method will be sent to the profiler manager host through this method.
    HRESULT STDMETHODCALLTYPE LogDumpMessage(_In_ const WCHAR* wszMessage);

private:
    HRESULT SetupOutputFile();
};

OBJECT_ENTRY_AUTO(__uuidof(CProfilerManagerHost), CProfilerManagerHost)
