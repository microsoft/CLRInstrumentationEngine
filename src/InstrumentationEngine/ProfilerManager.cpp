// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "ProfilerManager.h"
#include "MethodJitInfo.h"
#include "CorProfilerInfoWrapper.h"
#include "AssemblyInfo.h"
#include "CorProfilerFunctionControlWrapper.h"
#include "SignatureBuilder.h"
#include "TypeCreator.h"
#include "MethodInfoCleanup.h"
#include "InstrumentationMethod.h"
#include "ConfigurationLoader.h"
#include "InstrumentationMethodEvents.h"
#include <algorithm>

using namespace ATL;

// static
MicrosoftInstrumentationEngine::CProfilerManager* MicrosoftInstrumentationEngine::CProfilerManager::s_profilerManagerInstance = NULL;
//static
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetProfilerManagerInstance(_Out_ CProfilerManager** ppProfilerManager)
{
    HRESULT hr = S_OK;

    *ppProfilerManager = s_profilerManagerInstance;
    (*ppProfilerManager)->AddRef();

    return hr;
}

MicrosoftInstrumentationEngine::CProfilerManager::CProfilerManager() :
    m_bProfilingDisabled(false),
    m_dwEventMask(GetDefaultEventMask()),
    m_dwEventMaskHigh(0),
    m_attachedClrVersion(ClrVersion_Unknown),
    m_runtimeType((COR_PRF_RUNTIME_TYPE)0),
    m_bIsInInitialize(false),
    m_bIsInitializingInstrumentationMethod(false),
    m_dwInstrumentationMethodFlags(0),
    m_bValidateCodeSignature(true)
{
#ifdef PLATFORM_UNIX
    PAL_Initialize(0, NULL);
#endif

    WCHAR wszEnvVar[MAX_PATH];

#ifndef PLATFORM_UNIX
    GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_MessageboxAtAttach"), wszEnvVar, MAX_PATH);
    if (wcscmp(wszEnvVar, _T("1")) == 0)
    {
        MessageBox(NULL, _T("MicrosoftInstrumentationEngine ProfilerAttach"), L"", MB_OK);
    }
    if (GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_DebugWait"), wszEnvVar, MAX_PATH) > 0)
    {
        while (!IsDebuggerPresent())
        {
            Sleep(100);
        }
    }
#endif

    WCHAR wszLogLevel[MAX_PATH];
    ZeroMemory(wszLogLevel, MAX_PATH);
    bool fHasLogLevel = GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_LogLevel"), wszLogLevel, MAX_PATH) > 0;

    if (fHasLogLevel)
    {
        // Only want to support error logging in the event log, so specify that the allowed flags is Errors.
        // Currently, downstream dependencies also log only when this flag is set.
        LoggingFlags loggingType = ExtractLoggingFlags(wszLogLevel, LoggingFlags_Errors);

        if (loggingType != LoggingFlags_None)
        {
            CLogging::InitializeEventLogging();
            CLogging::SetLoggingFlags(loggingType);
        }
    }

    WCHAR wszFileLogLevel[MAX_PATH];
    ZeroMemory(wszFileLogLevel, MAX_PATH);
    bool fHasFileLogLevel = GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_FileLog"), wszFileLogLevel, MAX_PATH) > 0;

    WCHAR wszFileLogPath[MAX_PATH];
    ZeroMemory(wszFileLogPath, MAX_PATH);
    bool fHasFileLogPath = GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_FileLogPath"), wszFileLogPath, MAX_PATH) > 0;

    // Determine from where the file logging flags should be parsed.
    // File logging supports all log levels, so specify that the allowed flags is All.
    // File logging is enabled under two scenarios:
    // 1) If the FileLog env var is specified and it has a value that is something other than None, when parsed.
    // 2) If the LogLevel env var AND FileLogPath env var are specified and the LogLevel value is something other than None, when parsed.
    LoggingFlags fileLoggingFlags = LoggingFlags_None;
    if (fHasFileLogLevel)
    {
        fileLoggingFlags = ExtractLoggingFlags(wszFileLogLevel, LoggingFlags_All);
    }
    else if (fHasLogLevel && fHasFileLogPath)
    {
        fileLoggingFlags = ExtractLoggingFlags(wszLogLevel, LoggingFlags_All);
    }

    // Enable file logging
    if (fileLoggingFlags != LoggingFlags_None)
    {
        CLogging::EnableLoggingToFile(fileLoggingFlags, wszFileLogPath);
    }

#ifndef PLATFORM_UNIX
    //
    // Set this environment variable for testing purposes only.
    // Setting it will allow untrusted instrumentation methods to be loaded
    //
    // We consider this variable as secure as COR_ENABLE_PROFILER because we read it very early in process lifetime so malicious code
    // inside the process cannot modify it. Make sure you do not move this initialization logic and do not make it lazily initialized
    //
    if (GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_DisableCodeSignatureValidation"), nullptr, 0) > 0)
    {
        m_bValidateCodeSignature = false;
    }
#else
    m_bValidateCodeSignature = false;
#endif

    InitializeCriticalSection(&m_csForJIT);
    InitializeCriticalSection(&m_cs);
    InitializeCriticalSection(&m_csForAppDomains);

    // the PAL's implementation of COM does not invoke finalconstruct.
#ifdef PLATFORM_UNIX
    FinalConstruct();
#endif
}

MicrosoftInstrumentationEngine::CProfilerManager::~CProfilerManager()
{
    DeleteCriticalSection(&m_cs);
    CLogging::TerminateEventLogging();
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::FinalConstruct()
{
    HRESULT hr = S_OK;

#ifndef PLATFORM_UNIX
    // This object aggregates the free threaded marshaler which ensures all cross apartment calls to this object
    // do not marshal off the calling thead. In all likelihood, this shouldn't be a problem as I don't expect
    // any STA or MTA objects to get a hold of this. However, for correctness sake, I am doing this.
    ATL::CComPtr<IUnknown> pUnkMarshal;
    IfFailRet(CoCreateFreeThreadedMarshaler((IUnknown*)(ATL::CComObjectRootEx<ATL::CComMultiThreadModel>*)(this), (IUnknown**)&m_pFTM));
#endif

    m_pAppDomainCollection.Attach(new CAppDomainCollection);
    if (m_pAppDomainCollection == NULL)
    {
        return E_OUTOFMEMORY;
    }

    return hr;
}

void MicrosoftInstrumentationEngine::CProfilerManager::FinalRelease()
{

}


HRESULT MicrosoftInstrumentationEngine::CProfilerManager::SetupProfilingEnvironment(_In_reads_(numConfigPaths) BSTR rgConfigPaths[], _In_ UINT numConfigPaths)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(rgConfigPaths);

    for (UINT i = 0; i < numConfigPaths; i++)
    {
        m_configFilePaths.push_back(CComBSTR(rgConfigPaths[i]));
    }

    // The CLR doesn't initialize com before calling the profiler, and the profiler manager cannot do so itself
    // as that would screw up the com state for the application thread. This thread allows the profiler manager
    // to co create a free threaded version of msxml on a thread that it owns to avoid this.
    CHandle hConfigThread(CreateThread(NULL, 0, InstrumentationMethodThreadProc, this, 0, NULL));

    DWORD retVal = WaitForSingleObject(hConfigThread, 15000);
    if (retVal == WAIT_ABANDONED)
    {
        CLogging::LogError(_T("CProfilerManager::SetupProfilingEnvironment - instrumentation method configuration timeout exceeded"));
        return CORPROF_E_PROFILER_CANCEL_ACTIVATION;
    }
    else if (retVal == WAIT_FAILED)
    {
        CLogging::LogError(_T("CProfilerManager::SetupProfilingEnvironment - instrumentation method configuration failed"));
        return CORPROF_E_PROFILER_CANCEL_ACTIVATION;
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AddRawProfilerHook(
    _In_ IUnknown *pUnkProfilerCallback
    )
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pUnkProfilerCallback);

    if (m_profilerCallbackHolder != nullptr)
    {
        CLogging::LogError(_T("CAppDomainInfo::AddRawProfilerHook - Raw profiler hook is already initialized"));
        return E_FAIL;
    }

    if (!GetIsInInitialize())
    {
        CLogging::LogError(_T("Begin CAppDomainInfo::AddRawProfilerHook - Cannot add a raw profiler hook after initialize"));
        return E_FAIL;
    }

    CCriticalSectionHolder lock(&m_cs);

    unique_ptr<CProfilerCallbackHolder> profilerCallbackHolder(new CProfilerCallbackHolder);

    CComPtr<ICorProfilerCallback> pCorProfilerCallback;
    IfFailRet(pUnkProfilerCallback->QueryInterface(__uuidof(ICorProfilerCallback), (LPVOID*)&pCorProfilerCallback));
    profilerCallbackHolder->m_CorProfilerCallback = pCorProfilerCallback;

    CComPtr<ICorProfilerCallback2> pCorProfilerCallback2;
    hr = pUnkProfilerCallback->QueryInterface(__uuidof(ICorProfilerCallback2), (LPVOID*)&pCorProfilerCallback2);
    if (SUCCEEDED(hr))
    {
        profilerCallbackHolder->m_CorProfilerCallback2 = pCorProfilerCallback2;
    }

    CComPtr<ICorProfilerCallback3> pCorProfilerCallback3;
    hr = pUnkProfilerCallback->QueryInterface(__uuidof(ICorProfilerCallback3), (LPVOID*)&pCorProfilerCallback3);
    if (SUCCEEDED(hr))
    {
        profilerCallbackHolder->m_CorProfilerCallback3 = pCorProfilerCallback3;
    }

    CComPtr<ICorProfilerCallback4> pCorProfilerCallback4;
    hr = pUnkProfilerCallback->QueryInterface(__uuidof(ICorProfilerCallback4), (LPVOID*)&pCorProfilerCallback4);
    if (SUCCEEDED(hr))
    {
        profilerCallbackHolder->m_CorProfilerCallback4 = pCorProfilerCallback4;
    }

    CComPtr<ICorProfilerCallback5> pCorProfilerCallback5;
    hr = pUnkProfilerCallback->QueryInterface(__uuidof(ICorProfilerCallback5), (LPVOID*)&pCorProfilerCallback5);
    if (SUCCEEDED(hr))
    {
        profilerCallbackHolder->m_CorProfilerCallback5 = pCorProfilerCallback5;
    }

    CComPtr<ICorProfilerCallback6> pCorProfilerCallback6;
    hr = pUnkProfilerCallback->QueryInterface(__uuidof(ICorProfilerCallback6), (LPVOID*)&pCorProfilerCallback6);
    if (SUCCEEDED(hr))
    {
        profilerCallbackHolder->m_CorProfilerCallback6 = pCorProfilerCallback6;
    }

    CComPtr<ICorProfilerCallback7> pCorProfilerCallback7;
    hr = pUnkProfilerCallback->QueryInterface(__uuidof(ICorProfilerCallback7), (LPVOID*)&pCorProfilerCallback7);
    if (SUCCEEDED(hr))
    {
        profilerCallbackHolder->m_CorProfilerCallback7 = pCorProfilerCallback7;
    }

    m_profilerCallbackHolder = std::move(profilerCallbackHolder);

    if (m_attachedClrVersion != ClrVersion_2)
    {
        hr = pCorProfilerCallback->Initialize(m_pWrappedProfilerInfo);
    }
    else
    {
        hr = pCorProfilerCallback->Initialize(m_pRealProfilerInfo);
    }

    if (FAILED(hr))
    {
        CLogging::LogError(_T("Raw profiler hook returned failure"));
        return E_FAIL;
    }


    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RemoveRawProfilerHook(
    )
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder lock(&m_cs);

    m_profilerCallbackHolder = nullptr;

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetCorProfilerInfo(
    _Outptr_ IUnknown **ppCorProfiler
    )
{
    IfNullRetPointer(ppCorProfiler);
    IfNullRet(m_pWrappedProfilerInfo);

    *ppCorProfiler = (IUnknown*)(m_pWrappedProfilerInfo.p);
    (*ppCorProfiler)->AddRef();
    return S_OK;
}

// return the profiler host instance
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetProfilerHost(_Out_ IProfilerManagerHost** ppProfilerManagerHost)
{
    CCriticalSectionHolder holder(&m_cs);

    IfNullRetPointer(ppProfilerManagerHost);
    IfNullRet(m_profilerManagerHost);

    HRESULT hr = m_profilerManagerHost.CopyTo(ppProfilerManagerHost);
    return hr;
}

// Returns an instance of IProfilerManagerLogging which instrumentation methods can use
// to log to the profiler manager or optionally to the profiler host.
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetLoggingInstance(_Out_ IProfilerManagerLogging** ppLogging)
{
    *ppLogging = (IProfilerManagerLogging*)this;
    (*ppLogging)->AddRef();

    return S_OK;
}

// By default, logging messages are written to the debug output port. However,
// hosts can optionally signup to receive them through an instance of IProfilerManagerLogging
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::SetLoggingHost(_In_opt_ IProfilerManagerLoggingHost* pLoggingHost)
{
    HRESULT hr = S_OK;

    CLogging::SetLoggingHost(pLoggingHost);

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetAppDomainCollection(_Out_ IAppDomainCollection** ppAppDomainCollection)
{
    HRESULT hr = S_OK;
    *ppAppDomainCollection = (IAppDomainCollection*)m_pAppDomainCollection;
    (*ppAppDomainCollection)->AddRef();

    return hr;
}

// IProfilerManagerLogging Methods
// If trace logging in enabled in the profiler manager, trace messages are sent to the
// profiler manager through this function.
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::LogMessage(_In_ const WCHAR* wszMessage)
{
    CLogging::LogMessage(wszMessage);
    return S_OK;
}

// Errors detected during profiling will be sent to the host through this method
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::LogError(_In_ const WCHAR* wszError)
{
    CLogging::LogError(wszError);
    return S_OK;
}

// If instrumentation result tracing is enabled, the detailed results of each instrumented
// method will be sent to the profiler manager host through this method.
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::LogDumpMessage(_In_ const WCHAR* wszMessage)
{
    CLogging::LogDumpMessage(wszMessage);
    return S_OK;
}

// Called to cause logging to be written to the debug output port (via DebugOutputString) as well
// as to the host.
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::EnableDiagnosticLogToDebugPort(_In_ BOOL enable)
{
    CLogging::EnableDiagnosticLogToDebugPort(enable != 0);
    return S_OK;
}

// Allows instrumentation methods and hosts to ask for the current logging flags
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetLoggingFlags(_Out_ LoggingFlags* pLoggingFlags)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pLoggingFlags);

    IfFailRet(CLogging::GetLoggingFlags(pLoggingFlags));

    return S_OK;
}

// Allows instrumentation methods and hosts to modify the current logging level
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::SetLoggingFlags(_In_ LoggingFlags loggingFlags)
{
    HRESULT hr = S_OK;

    IfFailRet(CLogging::SetLoggingFlags(loggingFlags));

    return S_OK;
}

// The CLR doesn't initialize com before calling the profiler, and the profiler manager cannot do so itself
// as that would screw up the com state for the application thread. This thread allows the profiler manager
// to co create a free threaded version of msxml on a thread that it owns to avoid this.
DWORD WINAPI MicrosoftInstrumentationEngine::CProfilerManager::InstrumentationMethodThreadProc(
    _In_  LPVOID lpParameter
)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CProfilerManager::InstrumentationMethodThreadProc"));

#ifndef PLATFORM_UNIX
    hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        CLogging::LogError(_T("CProfilerManager::InstrumentationMethodThreadProc - CoInitializeEx failed"));
        return 1;
    }
#endif

    CProfilerManager* pProfilerManager = (CProfilerManager*)lpParameter;
    if (pProfilerManager == NULL)
    {
        CLogging::LogError(_T("CProfilerManager::InstrumentationMethodThreadProc - Invalid parameter"));
        return 1;
    }

    for (UINT i = 0; i < pProfilerManager->m_configFilePaths.size(); i++)
    {
        IfFailRet(pProfilerManager->LoadInstrumentationMethods(pProfilerManager->m_configFilePaths[i]));
    }

#ifndef PLATFORM_UNIX
    CoUninitialize();
#endif

    CLogging::LogMessage(_T("End CProfilerManager::InstrumentationMethodThreadProc"));

    return 0;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::LoadInstrumentationMethods(_In_ BSTR bstrConfigPath)
{
    HRESULT hr = S_OK;

    if (SysStringLen(bstrConfigPath) == 0)
    {
        CLogging::LogError(_T("Configuration xml should be set"));
        return E_FAIL;
    }

    CConfigurationLoader loader;

    std::vector<CInstrumentationMethod*> tempMethods;
    std::vector<CInstrumentationMethod*> instrumentationMethods;

    if (FAILED(loader.LoadConfiguration(bstrConfigPath, tempMethods)))
    {
        CLogging::LogError(_T("Failed to load configuration file '%s'."), bstrConfigPath);
    }

    // Remove methods with duplicate classIds
    for (CInstrumentationMethod* method : tempMethods)
    {
        GUID currentMethodGuid = method->GetClassId();
        std::vector<GUID>::iterator it = std::find(m_instrumentationMethodGuids.begin(), m_instrumentationMethodGuids.end(), currentMethodGuid);
        if (it != m_instrumentationMethodGuids.end())
        {
            WCHAR wszCurrentMethodGuid[40] = { 0 };
            if (0 != ::StringFromGUID2(currentMethodGuid, wszCurrentMethodGuid, 40))
            {
                CLogging::LogMessage(_T("CProfilerManager::LoadInstrumentationMethods - Instrumentation Method found with duplicate ClassId '%s' of another previously loaded method. Skipping."), wszCurrentMethodGuid);
            }

            delete method;
        }
        else
        {
            instrumentationMethods.push_back(method);
            m_instrumentationMethodGuids.push_back(currentMethodGuid);
        }
    }

    for (CInstrumentationMethod* method : instrumentationMethods)
    {
        IInstrumentationMethod* pInstrumentationMethod = nullptr;
        this->AddInstrumentationMethod(method, &pInstrumentationMethod);
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RemoveInstrumentationMethod(
    _In_ IInstrumentationMethod* pInstrumentationMethod)
{
    HRESULT hr = S_OK;
    {
        DWORD dwFlags = 0;
        CCriticalSectionHolder lock(&m_cs);
        for (TInstrumentationMethodsCollection::iterator iter = m_instrumentationMethods.begin(); iter != m_instrumentationMethods.end(); iter++)
        {
            CComPtr<IInstrumentationMethod> spInstrMethodTmp;
            IfFailRet(iter->first->GetRawInstrumentationMethod(&spInstrMethodTmp));
            if (spInstrMethodTmp.p == pInstrumentationMethod)
            {
                dwFlags = iter->second;
                m_instrumentationMethods.erase(iter);
                break;
            }
        }
        if (dwFlags != 0)
        {
            CInitializeInstrumentationMethodHolder initHolder(this);

            //this operation will remove flags associated with removed Instrumentation method
            // We need to trace error, however remove operation will succeed anyway
            HRESULT hr2 = SetEventMask(0);
            if (FAILED(hr2))
            {
#ifndef PLATFORM_UNIX
                CLogging::LogError(_T("SetEventMask failed in function ") __FUNCTIONW__ _T(". This may be expected if instrumentation method attempted to set mutable flags. Flags: %X %X"), dwFlags, hr2);
#else
                CLogging::LogError(_T("SetEventMask failed in function ") WCHAR_SPEC _T(". This may be expected if instrumentation method attempted to set mutable flags. Flags: %X %X"), __FUNCTION__, dwFlags, hr);
#endif
            }
        }
    }
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AddInstrumentationMethod(
    _In_ CInstrumentationMethod* pInstrumentationMethod,
    _Out_ IInstrumentationMethod** ppInstrumentationMethod)
{
    HRESULT hr = S_OK;
    {
        CCriticalSectionHolder lock(&m_cs);
        DWORD dwFlags = 0;

        {
            //Initialize Instrumentation method.
            // Initialize method of InstrumentationMethod can set additional flags that will be associated with this
            // InstrumentationMethod. Since we are using global lock - InstrumentationMethod can only set flags associated
            // with it in this thread.
            CInitializeInstrumentationMethodHolder initHolder(this);
            hr = pInstrumentationMethod->Initialize(this, m_bValidateCodeSignature);
            dwFlags = GetInitializingInstrumentationMethodFlags();
        }

        if (FAILED(hr))
        {
            return hr;
        }

        bool bInserted = false;

        // Number of instrumentation methods is expected to be very small.
        for (TInstrumentationMethodsCollection::iterator iter = m_instrumentationMethods.begin(); iter != m_instrumentationMethods.end(); iter++)
        {
            shared_ptr<CInstrumentationMethod> pCurrInstrumentationMethod = iter->first;

            if (pCurrInstrumentationMethod->GetPriority() > pInstrumentationMethod->GetPriority())
            {
                m_instrumentationMethods.insert(iter, TInstrumentationMethodsCollection::value_type(std::shared_ptr<CInstrumentationMethod>(pInstrumentationMethod), dwFlags));
                bInserted = true;
                break;
            }
        }

        if (!bInserted)
        {
            m_instrumentationMethods.push_back(TInstrumentationMethodsCollection::value_type(std::shared_ptr<CInstrumentationMethod>(pInstrumentationMethod), 0));
        }

    }

    IfFailRet(pInstrumentationMethod->GetRawInstrumentationMethod(ppInstrumentationMethod));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AddInstrumentationMethod(
    _In_ BSTR bstrModulePath,
    _In_ BSTR bstrName,
    _In_ BSTR bstrDescription,
    _In_ BSTR bstrModule,
    _In_ BSTR bstrClassGuid,
    _In_ DWORD dwPriority,
    _Out_ IInstrumentationMethod** ppInstrumentationMethod)
{
    IfNullRetPointer(ppInstrumentationMethod);
    *ppInstrumentationMethod = nullptr;

    HRESULT hr = S_OK;

    GUID guidClassId;
    hr = IIDFromString(bstrClassGuid, (LPCLSID)&guidClassId);
    if (FAILED(hr))
    {
        CLogging::LogError(_T("CInstrumentationMethod::Initialize - Bad classid for instrumentation method"));
        return E_INVALIDARG;
    }

    unique_ptr<CInstrumentationMethod> pInstrumentationMethod(new CInstrumentationMethod(bstrModulePath, bstrName, bstrDescription, bstrModule, guidClassId, dwPriority));
    if (pInstrumentationMethod == nullptr)
    {
        CLogging::LogError(_T("Unable to create a new instance of CInstrumentationMethod. Treating it as an out of memory error."));
        return E_OUTOFMEMORY;
    }

    return this->AddInstrumentationMethod(pInstrumentationMethod.release(), ppInstrumentationMethod);
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::DisableProfiling()
{
    m_bProfilingDisabled = true;

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ApplyMetadata(_In_ IModuleInfo* pMethodInfo)
{
    HRESULT hr = S_OK;

    IfNullRet(pMethodInfo);

    CComQIPtr<ICorProfilerInfo7> pCorProfiler7 = m_pRealProfilerInfo.p;
    IfNullRet(pCorProfiler7);

    ModuleID moduleId;
    IfFailRet(static_cast<CModuleInfo*>(pMethodInfo)->GetModuleID(&moduleId));

    IfFailRet(pCorProfiler7->ApplyMetaData(moduleId));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetApiVersion(_Out_ DWORD* pApiVer)
{
    IfNullRet(pApiVer);

    *pApiVer = CLR_INSTRUMENTATION_ENGINE_API_VER;
    return S_OK;
}

// ICorProfilerCallback methods
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::Initialize(
    _In_ IUnknown *pICorProfilerInfoUnk)
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    // Mark that this is during the initialize call. This enables operations that can only be supported during initialize
    CInitializeHolder initHolder(this);

    if (s_profilerManagerInstance != NULL)
    {
        return CORPROF_E_PROFILER_CANCEL_ACTIVATION;
    }
    s_profilerManagerInstance = this;

    // TODO: this will be called once for each instance of the clr in the process.
    // For in-proc sxs, this means the host may create multiple instances of this object.
    // TODO: Need to figure out how to deal with this. Colin believes "choose first" may be good enough.
    // However, need feedback from Tofino on this issue.
    //
    // See David Browman's blog on the issue:
    // http://blogs.msdn.com/b/davbr/archive/2010/08/25/profilers-in-process-side-by-side-clr-instances-and-a-free-test-harness.aspx

    IfFailRet(pICorProfilerInfoUnk->QueryInterface(__uuidof(ICorProfilerInfo), (LPVOID*)&m_pRealProfilerInfo));

    IfFailRet(DetermineClrVersion());

    //set event mask to be default one. Later host and Instrumentation methods may call it again
    IfFailRet(this->SetEventMask(m_dwEventMask));

    m_profilerManagerHost.Attach(new CExtensionsHost);
    IfFailRet(m_profilerManagerHost == nullptr ? E_OUTOFMEMORY : S_OK);

    m_pWrappedProfilerInfo = (ICorProfilerInfo*)(new CCorProfilerInfoWrapper(this, m_pRealProfilerInfo));

    // Initialize the profiler manager host. This gives the host the chance to initialize, configure instrumentation methods, and register for raw events if desired.
    hr = m_profilerManagerHost->Initialize(this);
    if (FAILED(hr))
    {
        CLogging::LogError(_T("CInstrumentationMethod::Initialize - failed to initialize profiler manager host"));
        return hr;
    }

    if (m_bProfilingDisabled)
    {
        return CORPROF_E_PROFILER_CANCEL_ACTIVATION;
    }

    // Take the lock that products the raw profiler callback and the instrumentation methods. This keeps the collection from changing out from under the iterator

    CCriticalSectionHolder lock(&m_cs);

    if (m_profilerCallbackHolder != nullptr)
    {
        CComPtr<ICorProfilerCallback> pCallback = m_profilerCallbackHolder->m_CorProfilerCallback;
        if (m_attachedClrVersion != ClrVersion_2)
        {
            pCallback->Initialize((IUnknown*)(m_pWrappedProfilerInfo.p));
        }
        else
        {
            pCallback->Initialize((IUnknown*)(m_pRealProfilerInfo.p));
        }
    }

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::DetermineClrVersion()
{
    if (m_pRealProfilerInfo)
    {
        CComPtr<ICorProfilerInfo3> pRealProfilerInfo3;
        if (FAILED(m_pRealProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo3), (void**)&pRealProfilerInfo3)) ||
            pRealProfilerInfo3 == nullptr ||
            FAILED(pRealProfilerInfo3->GetRuntimeInformation(
                nullptr,
                &m_runtimeType,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                0,
                nullptr,
                nullptr)))
        {
            m_runtimeType = COR_PRF_DESKTOP_CLR;
        }

        // Get ICorProfilerInfo, use to determine whether it's CLRv2 or CLRv4+.
        // TODO-leonidd-080923: Use GetRuntimeInfo when available to set the CLR version
        if (CComQIPtr<ICorProfilerInfo4> pi4 = m_pRealProfilerInfo.p)
        {
            m_attachedClrVersion = ClrVersion_4_5; // v4.5
            return S_OK;
        }
        else if (CComQIPtr<ICorProfilerInfo3> pi3 = m_pRealProfilerInfo.p)
        {
            m_attachedClrVersion = ClrVersion_4; // v4.0
            return S_OK;
        }
        else if (CComQIPtr<ICorProfilerInfo2> pi2 = m_pRealProfilerInfo.p)
        {
            m_attachedClrVersion = ClrVersion_2; // v2.0
            return S_OK;
        }
    }

    return CORPROF_E_PROFILER_CANCEL_ACTIVATION;
}

void MicrosoftInstrumentationEngine::CProfilerManager::SetIsInInitialize(_In_ bool val)
{
    m_bIsInInitialize = val;
}

bool MicrosoftInstrumentationEngine::CProfilerManager::GetIsInInitialize() const
{
    return m_bIsInInitialize;
}

void MicrosoftInstrumentationEngine::CProfilerManager::SetIsInitializingInstrumentationMethod(_In_ bool val)
{
    m_bIsInitializingInstrumentationMethod = val;
    m_dwInstrumentationMethodFlags = 0;
}

bool MicrosoftInstrumentationEngine::CProfilerManager::GetIsInitializingInstrumentationMethod() const
{
    return m_bIsInitializingInstrumentationMethod;
}

void MicrosoftInstrumentationEngine::CProfilerManager::SetInitializingInstrumentationMethodFlags(_In_ DWORD dwFlags)
{
    m_dwInstrumentationMethodFlags = dwFlags;
}

DWORD MicrosoftInstrumentationEngine::CProfilerManager::GetInitializingInstrumentationMethodFlags() const
{
    return m_dwInstrumentationMethodFlags;
}


HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetEventMask(_Out_ DWORD* dwEventMask)
{
    HRESULT hr = S_OK;
    // NOTE: holding the critical section just in case some one calls Get from another thread
    // while Set is executing.
    CCriticalSectionHolder lock(&m_cs);

    IfFailRet(m_pRealProfilerInfo->GetEventMask(dwEventMask));

    return hr;
}

DWORD MicrosoftInstrumentationEngine::CProfilerManager::CalculateEventMask(DWORD dwAdditionalFlags)
{
    DWORD result = m_dwEventMask | dwAdditionalFlags;

    CCriticalSectionHolder lock(&m_cs);

    TInstrumentationMethodsCollection::const_iterator it;
    for (it = m_instrumentationMethods.begin(); it != m_instrumentationMethods.end(); ++it)
    {
        result |= (*it).second;
    }

    return result;
}


HRESULT MicrosoftInstrumentationEngine::CProfilerManager::SetEventMask(DWORD dwEventMask)
{
    HRESULT hr = S_OK;

    //This is important to set critical section here. AddInstrumentationMethod will
    //associate flags set by instrumentation method during initialization with this instrumentation method
    CCriticalSectionHolder lock(&m_cs);

    DWORD dwAdditionalFlags = 0;

    if (GetIsInitializingInstrumentationMethod())
    {
        SetInitializingInstrumentationMethodFlags(dwEventMask);
        dwAdditionalFlags = dwEventMask;
    }
    else if (GetIsInInitialize())
    {
        m_dwEventMask |= dwEventMask;
    }
    else
    {
        CLogging::LogError(_T("SetEventMask can only be called during initialize of Host or InstrumentationMethod"));
        return E_FAIL;
    }

    CComQIPtr<ICorProfilerInfo5> pCorProfilerInfo5 = m_pRealProfilerInfo.p;
    if (pCorProfilerInfo5 != nullptr)
    {
        IfFailRet(pCorProfilerInfo5->SetEventMask2(CalculateEventMask(dwAdditionalFlags), m_dwEventMaskHigh));
    }
    else
    {
        IfFailRet(m_pRealProfilerInfo->SetEventMask(CalculateEventMask(dwAdditionalFlags)));
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetEventMask2(_Out_ DWORD* dwEventMaskLow, _Out_ DWORD* dwEventMaskHigh)
{
    CComQIPtr<ICorProfilerInfo5> pCorProfilerInfo5 = m_pRealProfilerInfo.p;
    IfNullRet(pCorProfilerInfo5);

    return pCorProfilerInfo5->GetEventMask2(dwEventMaskLow, dwEventMaskHigh);
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::SetEventMask2(_In_ DWORD dwEventMaskLow, _In_ DWORD dwEventMaskHigh)
{
    HRESULT hr = S_OK;

    m_dwEventMaskHigh |= dwEventMaskHigh;
    IfFailRet(SetEventMask(dwEventMaskLow));

    return S_OK;
}

MicrosoftInstrumentationEngine::ClrVersion MicrosoftInstrumentationEngine::CProfilerManager::GetAttachedClrVersion()
{
    return m_attachedClrVersion;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetRuntimeType(_Out_ COR_PRF_RUNTIME_TYPE* pRuntimeType)
{
    IfNullRetPointer(pRuntimeType);
    *pRuntimeType = m_runtimeType;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetRealCorProfilerInfo(ICorProfilerInfo** ppRealProfilerInfo)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppRealProfilerInfo);
    *ppRealProfilerInfo = NULL;

    hr = m_pRealProfilerInfo.CopyTo(ppRealProfilerInfo);

    return S_OK;
}

// Set the default event mask. This is union'd with the event mask from instrumentation methods and the host.
//static
DWORD MicrosoftInstrumentationEngine::CProfilerManager::GetDefaultEventMask()
{
    return
        COR_PRF_MONITOR_APPDOMAIN_LOADS |
        COR_PRF_MONITOR_ASSEMBLY_LOADS |
        COR_PRF_MONITOR_MODULE_LOADS |
        COR_PRF_MONITOR_JIT_COMPILATION |
        COR_PRF_ENABLE_REJIT
        ;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::Shutdown()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    CCriticalSectionHolder holder(&m_cs);

    // Send the event to instrumentation methods.
    SendEventToInstrumentationMethods(&IInstrumentationMethod::OnShutdown);

    // Cleanup the appdomains. This is done before cleaning up the host so logging is still
    // enabled.
    if (m_pAppDomainCollection != NULL)
    {
        m_pAppDomainCollection->OnShutdown();
    }

    // Release the logging host to ensure the pointer to the host dll is cleared before the actual shutdown.
    // MMA will AV if this is done after SendEventToRawProfilerCallback
    CLogging::SetLoggingHost(nullptr);

    // send raw event to registered callbacks
    // Do not touch the logging host after this point.
    // Many hosts will assume they can clean everything up after this call.
    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::Shutdown));

    // Unloading instrumentation methods
    m_instrumentationMethods.clear();

    // DO NOT Release the profiler manager host instance.
    // The CLR does not release this pointer during shutdown, and most hosts do all
    // of their cleanup during the Shutdown event. Detach the pointer and allow it
    // to leak the reference.
    m_profilerManagerHost.Detach();

    PROF_CALLBACK_END

    return S_OK;
}


HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AppDomainCreationStarted(
    _In_ AppDomainID appDomainId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    // NOTE: In all likelyhood, the appdomain was already created as it is needed for assembly and module loads
    CComPtr<IAppDomainInfo> pAppDomainInfo;
    hr = m_pAppDomainCollection->GetAppDomainById(appDomainId, &pAppDomainInfo);
    if (FAILED(hr))
    {
        IfFailRet(ConstructAppDomainInfo(appDomainId, &pAppDomainInfo));
    }

    IGNORE_IN_NET20_END

    // send raw event to registered callbacks
    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::AppDomainCreationStarted, appDomainId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AppDomainCreationFinished(
    _In_ AppDomainID appDomainId,
    _In_ HRESULT hrStatus
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    //Return if the appdomain didn't load correctly
    if (FAILED (hrStatus))
    {
        CLogging::LogMessage(_T("AppDomainCreationFinished bailing out, FAILED hrStatus given"));
        return S_OK;
    };

    CComPtr<IAppDomainInfo> pAppDomainInfo;
    IfFailRet(m_pAppDomainCollection->GetAppDomainById(appDomainId, &pAppDomainInfo));

    CAppDomainInfo* pRawAppDomainInfo = (CAppDomainInfo*)(pAppDomainInfo.p);

    IfFailRet(pRawAppDomainInfo->FinishInitialization(m_pRealProfilerInfo));

    // Send event to instrumentation method
    SendEventToInstrumentationMethods(&IInstrumentationMethod::OnAppDomainCreated, (IAppDomainInfo*)(pAppDomainInfo));

    IGNORE_IN_NET20_END

    // send raw event to registered callbacks
    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::AppDomainCreationFinished, appDomainId, hrStatus));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AppDomainShutdownStarted(
    _In_ AppDomainID appDomainId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    // send raw event to registered callbacks
    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::AppDomainShutdownStarted, appDomainId));

    PROF_CALLBACK_END

    return S_OK;
}


HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AppDomainShutdownFinished(
    _In_ AppDomainID appDomainId,
    _In_ HRESULT hrStatus
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    // Because the CLR does not guarentee the order of appdomain shutdown and assembly / module unloads.
    // block those calls for the duration.
    CCriticalSectionHolder holder(&m_csForAppDomains);

    IGNORE_IN_NET20_BEGIN

    CComPtr<IAppDomainInfo> pAppDomainInfo;
    IfFailRet(m_pAppDomainCollection->GetAppDomainById(appDomainId, &pAppDomainInfo));

    CAppDomainInfo* pRawAppdomainInfo = (CAppDomainInfo*)pAppDomainInfo.p;

    // The CLR waits until long after the appdomain shutdown event to send the assembly unload and module unload events.
    // This is a problem because there is no final event to notify the profiler that all unloads have happened so the
    // appdomain can be cleaned up.
    // So, manufacture the unload events for the modules and assemblies now. This will break the circular reference between them
    // and the appdomain. When the real event comes in, it will be ignored if this event already fired.
    // Note that it is possible in some scenarios for the callback to be reversed. In which case, the assemblies and modules
    // will unload first and this collection will have no elements in it.
    SendFabricatedModuleUnloads(pRawAppdomainInfo);
    SendFabricatedAssemblyUnloads(pRawAppdomainInfo);

    // Send the appdomain event to instrumentation methods
    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethod::OnAppDomainShutdown, (IAppDomainInfo*)(pAppDomainInfo)));

    // Remove the appdomain from the appdomain collection.
    IfFailRet(m_pAppDomainCollection->RemoveAppDomainInfo(appDomainId));
    IGNORE_IN_NET20_END

    // send raw event to registered callbacks
    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::AppDomainShutdownFinished, appDomainId, hrStatus));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::SendFabricatedModuleUnloads(_In_ CAppDomainInfo* pRawAppdomainInfo)
{
    HRESULT hr = S_OK;

    shared_ptr<list<ModuleID>> pModuleIds = pRawAppdomainInfo->GetModuleIds();
    for (ModuleID moduleId : *pModuleIds)
    {
        IfFailRet(ModuleUnloadStartedImpl(moduleId));
        IfFailRet(ModuleUnloadFinishedImpl(moduleId, S_OK));
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::SendFabricatedAssemblyUnloads(_In_ CAppDomainInfo* pRawAppdomainInfo)
{
    HRESULT hr = S_OK;

    shared_ptr<list<AssemblyID>> pAssemblyIds = pRawAppdomainInfo->GetAssemblyIds();
    for (AssemblyID assemblyId : *pAssemblyIds)
    {
        IfFailRet(AssemblyUnloadStartedImpl(assemblyId));
        IfFailRet(AssemblyUnloadFinishedImpl(assemblyId, S_OK));
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ModuleUnloadStartedImpl(_In_ ModuleID moduleId)
{
    HRESULT hr = S_OK;

    // Because the CLR does not guarentee the order of appdomain shutdown and assembly / module unloads.
    // block those calls for the duration.
    CCriticalSectionHolder holder(&m_csForAppDomains);

    IGNORE_IN_NET20_BEGIN

    CComPtr<IModuleInfo> pModuleInfo;
    hr = m_pAppDomainCollection->GetModuleInfoById(moduleId, &pModuleInfo);
    if (FAILED(hr))
    {
        // Module no longer exists as the appdomain shutdown event already handled cleanup.
        return S_OK;
    }

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ModuleUnloadStarted, moduleId));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ModuleUnloadFinishedImpl(_In_ ModuleID moduleId, _In_ HRESULT hrStatus)
{
    HRESULT hr = S_OK;

    // Because the CLR does not guarentee the order of appdomain shutdown and assembly / module unloads.
    // block those calls for the duration.
    CCriticalSectionHolder holder(&m_csForAppDomains);
    CComPtr<IModuleInfo> pModuleInfo;

    IGNORE_IN_NET20_BEGIN
    hr = m_pAppDomainCollection->GetModuleInfoById(moduleId, &pModuleInfo);
    if (FAILED(hr))
    {
        // Module no longer exists as the appdomain shutdown event already handled cleanup.
        return S_OK;
    }

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethod::OnModuleUnloaded, (IModuleInfo*)(pModuleInfo)));
    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ModuleUnloadFinished, moduleId, hrStatus));

    IGNORE_IN_NET20_BEGIN
    CComPtr<IAppDomainInfo> pAppDomainInfo;
    IfFailRet(pModuleInfo->GetAppDomainInfo(&pAppDomainInfo));

    CComPtr<IAssemblyInfo> pAssemblyInfo;
    IfFailRet(pModuleInfo->GetAssemblyInfo(&pAssemblyInfo));

    CAppDomainInfo* pRawAppDomainInfo = static_cast<CAppDomainInfo*>(pAppDomainInfo.p);
    CModuleInfo* pRawModuleInfo = static_cast<CModuleInfo*>(pModuleInfo.p);
    CAssemblyInfo* pRawAssemblyInfo = static_cast<CAssemblyInfo*>(pAssemblyInfo.p);

    IfFailRet(pRawAssemblyInfo->ModuleInfoUnloaded(pRawModuleInfo));
    IfFailRet(pRawAppDomainInfo->ModuleInfoUnloaded(pRawModuleInfo));

    // Let the module info cleanup the remaining circular references.
    IfFailRet(pRawModuleInfo->Dispose());
    IGNORE_IN_NET20_END

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AssemblyUnloadStartedImpl(_In_ AssemblyID assemblyId)
{
    HRESULT hr = S_OK;

    // Because the CLR does not guarentee the order of appdomain shutdown and assembly / module unloads.
    // block those calls for the duration.
    CCriticalSectionHolder holder(&m_csForAppDomains);

    IGNORE_IN_NET20_BEGIN

    CComPtr<IAssemblyInfo> pAssemblyInfo;
    hr = m_pAppDomainCollection->GetAssemblyInfoById(assemblyId, &pAssemblyInfo);
    if (FAILED(hr))
    {
        // Assembly no longer exists as the appdomain shutdown event already handled cleanup.
        return S_OK;
    }

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::AssemblyUnloadStarted, assemblyId));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AssemblyUnloadFinishedImpl(_In_ AssemblyID assemblyId, _In_ HRESULT hrStatus)
{
    HRESULT hr = S_OK;

    // Because the CLR does not guarentee the order of appdomain shutdown and assembly / module unloads.
    // block those calls for the duration.
    CCriticalSectionHolder holder(&m_csForAppDomains);

    IGNORE_IN_NET20_BEGIN

    CComPtr<IAssemblyInfo> pAssemblyInfo;
    hr = m_pAppDomainCollection->GetAssemblyInfoById(assemblyId, &pAssemblyInfo);
    if (FAILED(hr))
    {
        // Assembly no longer exists as the appdomain shutdown event already handled cleanup.
        return S_OK;
    }

    CComPtr<IAppDomainInfo> pAppDomainInfo;
    IfFailRet(pAssemblyInfo->GetAppDomainInfo(&pAppDomainInfo));

    CAppDomainInfo* pRawAppDomainInfo = static_cast<CAppDomainInfo*>(pAppDomainInfo.p);
    CAssemblyInfo* pRawAssemblyInfo = static_cast<CAssemblyInfo*>(pAssemblyInfo.p);

    IfFailRet(pRawAppDomainInfo->AssemblyInfoUnloaded(pRawAssemblyInfo));

    // Send event to instrumentation methods
    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethod::OnAssemblyUnloaded, (IAssemblyInfo*)(pAssemblyInfo)));

    IGNORE_IN_NET20_END

    // Send the event to the raw callbacks
    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::AssemblyUnloadFinished, assemblyId, hrStatus));

    return hr;
}

LoggingFlags MicrosoftInstrumentationEngine::CProfilerManager::ExtractLoggingFlags(
    _In_ LPCWSTR wszRequestedFlagNames,
    _In_ LoggingFlags allowedFlags
    )
{
    if (nullptr == wszRequestedFlagNames)
    {
        return LoggingFlags_None;
    }

    // If the request is for all logging flags, just return the allowable flags
    if (wcsstr(wszRequestedFlagNames, _T("All")) != nullptr)
    {
        return allowedFlags;
    }

    // For each logging flag, check that the named flag was specified and that the log flag is something
    // that we are actually trying to find. Combine the results of each (since each result will be a
    // single flag) and return the combination.
    return (LoggingFlags)(
        ExtractLoggingFlag(wszRequestedFlagNames, allowedFlags, _T("Errors"), LoggingFlags_Errors) |
        ExtractLoggingFlag(wszRequestedFlagNames, allowedFlags, _T("Messages"), LoggingFlags_Trace) |
        ExtractLoggingFlag(wszRequestedFlagNames, allowedFlags, _T("Dumps"), LoggingFlags_InstrumentationResults)
        );
}

LoggingFlags MicrosoftInstrumentationEngine::CProfilerManager::ExtractLoggingFlag(
    _In_ LPCWSTR wszRequestedFlagNames,
    _In_ LoggingFlags allowedFlags,
    _In_ LPCWSTR wszSingleTestFlagName,
    _In_ LoggingFlags singleTestFlag
    )
{
    if (nullptr == wszRequestedFlagNames || nullptr == wszSingleTestFlagName)
    {
        return LoggingFlags_None;
    }

    // Test that the desired flag is in the combination of flags that are allowed to be checked.
    // Additionally, check that the name of the flag is in the string that specifies what types
    // of messages are requested to be logged.
    if (IsFlagSet(allowedFlags, singleTestFlag) && wcsstr(wszRequestedFlagNames, wszSingleTestFlagName) != nullptr)
    {
        return singleTestFlag;
    }
    return LoggingFlags_None;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AssemblyLoadStarted(
    _In_ AssemblyID assemblyId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    // send raw event to registered callbacks
    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::AssemblyLoadStarted, assemblyId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AssemblyLoadFinished(
    _In_ AssemblyID assemblyId,
    _In_ HRESULT hrStatus
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    CComPtr<IAssemblyInfo> pAssemblyInfo;
    IfFailRet(ConstructAssemblyInfo(assemblyId, &pAssemblyInfo));

    // Send event to instrumentation methods
    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethod::OnAssemblyLoaded, (IAssemblyInfo*)(pAssemblyInfo)));

    IGNORE_IN_NET20_END

    // send raw event to registered callbacks
    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::AssemblyLoadFinished, assemblyId, hrStatus));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AssemblyUnloadStarted(
    _In_ AssemblyID assemblyId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(AssemblyUnloadStartedImpl(assemblyId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::AssemblyUnloadFinished(
    _In_ AssemblyID assemblyId,
    _In_ HRESULT hrStatus
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(AssemblyUnloadFinishedImpl(assemblyId, hrStatus));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ModuleLoadStarted(
    _In_ ModuleID moduleId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    // send raw event to registered callbacks
    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ModuleLoadStarted, moduleId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ModuleLoadFinished(
    _In_ ModuleID moduleId,
    _In_ HRESULT hrStatus
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ModuleLoadFinished, moduleId, hrStatus));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ModuleUnloadStarted(
    _In_ ModuleID moduleId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(ModuleUnloadStartedImpl(moduleId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ModuleUnloadFinished(
    _In_ ModuleID moduleId,
    _In_ HRESULT hrStatus
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(ModuleUnloadFinishedImpl(moduleId, hrStatus));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ModuleAttachedToAssembly(
    _In_ ModuleID moduleId,
    _In_ AssemblyID AssemblyId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    CComPtr<IModuleInfo> pModuleInfo;
    // NOTE: it is expected that ConstructModuleInfo will fail for resource only modules.
    hr = ConstructModuleInfo(moduleId, &pModuleInfo);
    if (SUCCEEDED(hr))
    {
        // Send event to instrumentation methods
        IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethod::OnModuleLoaded, (IModuleInfo*)(pModuleInfo)));
    }

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ModuleAttachedToAssembly, moduleId, AssemblyId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ClassLoadStarted(
    _In_ ClassID classId)
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ClassLoadStarted, classId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ClassLoadFinished(
    _In_ ClassID classId,
    _In_ HRESULT hrStatus
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ClassLoadFinished, classId, hrStatus));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ClassUnloadStarted(
    _In_ ClassID classId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ClassUnloadStarted, classId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ClassUnloadFinished(
    _In_ ClassID classId,
    _In_ HRESULT hrStatus
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ClassUnloadFinished, classId, hrStatus));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::FunctionUnloadStarted(
    _In_ FunctionID functionId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::FunctionUnloadStarted, functionId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::JITCompilationStarted(
    _In_ FunctionID functionId,
    _In_ BOOL fIsSafeToBlock
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

        if (m_attachedClrVersion != ClrVersion_2)
        {
            CComPtr<CMethodInfo> pInformationalMethodInfo;

            IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodJitEvents::JitStarted, functionId, FALSE));

            // Only allow a single method to be instrumented at a time. Note that the clr sometimes calls JITCompilationStarted
            // multiple times with same function at the same time, and instrumenting simultainously could result in duplicate
            // copies. This ensures only a single instrumentation pass happens for each method info instance.
            //
            // NOTE: this critical section is used in this callback and in JITInline
            // m_cs will only be blocked for a short duration. So other callbacks will not be blocked for the time of JIT compiling of the method
            CCriticalSectionHolder holder(&m_csForJIT);

            CComPtr<CMethodInfo> pMethodInfo;
            hr = CreateMethodInfo(functionId, &pMethodInfo);
            ClearILTransformationStatus(functionId);

            // Class to call cleanup on the method info in the destructor.
            CCleanupMethodInfo cleanupMethodInfo(pMethodInfo);

            if (SUCCEEDED(hr))
            {
                CComPtr<IModuleInfo> pModuleInfo;
                IfFailRet(pMethodInfo->GetModuleInfo(&pModuleInfo));

                BOOL isDynamic = false;
                IfFailRet(pModuleInfo->GetIsDynamic(&isDynamic));

                // We cannot instrument modules that have no image base
                if (!isDynamic)
                {
                    //Get Name Here and output
                    CComBSTR name;
                    if (SUCCEEDED(pMethodInfo->GetFullName(&name)))
                    {
                        CLogging::LogMessage(_T("JITCompilation FullMethodName %s"), name.m_str);
                    }
                    else
                    {
                        CLogging::LogMessage(_T("Method name failed"));
                    }

                    // Query if the instrumentation methods want to instrument and then have them actually instrument.
                    vector<CComPtr<IInstrumentationMethod>> toInstrument;
                    IfFailRet(CallShouldInstrumentOnInstrumentationMethods(pMethodInfo, FALSE, &toInstrument));

                    // Give the instrumentation methods a chance to do method body replacement. Only one can replace the
                    // method body
                    IfFailRet(CallBeforeInstrumentMethodOnInstrumentationMethods(pMethodInfo, FALSE, toInstrument));

                    // Instrumentation methods to most of their instrumentation during InstrumentMethod
                    IfFailRet(CallInstrumentOnInstrumentationMethods(pMethodInfo, FALSE, toInstrument));

                    // Send the event to the raw callbacks to give them a chance to instrument.
                    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::JITCompilationStarted, functionId, fIsSafeToBlock));

                    // If the method was instrumetned
                    if (pMethodInfo->IsInstrumented())
                    {
                        // Give the final instrumentation to the clr.
                        IfFailRet(pMethodInfo->ApplyFinalInstrumentation());

                        // Don't fail out with this call. It is too late to undo anything with a failure.
                        CallOnInstrumentationComplete(pMethodInfo, false);
                    }
                }
            }
        }
        else
        {
            // Send the event to the raw callbacks to give them a chance to instrument.
            IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::JITCompilationStarted, functionId, fIsSafeToBlock));
        }

    PROF_CALLBACK_END

    return S_OK;
}


HRESULT MicrosoftInstrumentationEngine::CProfilerManager::JITCompilationFinished(
    _In_ FunctionID functionId,
    _In_ HRESULT hrStatus,
    _In_ BOOL fIsSafeToBlock
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    CComPtr<CMethodJitInfo> pMethodJitInfo;
    pMethodJitInfo.Attach(new CMethodJitInfo(functionId, hrStatus, false, 0, this));
    IfFailRet(ForEachInstrumentationMethod(Events::SendJitCompleteEvent, pMethodJitInfo));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::JITCompilationFinished, functionId, hrStatus, fIsSafeToBlock));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::JITCachedFunctionSearchStarted(
    _In_ FunctionID functionId,
    /* [out] */ BOOL *pbUseCachedFunction
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::JITCachedFunctionSearchStarted, functionId, pbUseCachedFunction));

    // TODO: I think if any callback says not to use the cached function, the cached
    // function should be disabled. For now, I'm just overriding what the callback say
    // and returning false thus blocking ngen
    *pbUseCachedFunction = FALSE;

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::JITCachedFunctionSearchFinished(
    _In_ FunctionID functionId,
    _In_ COR_PRF_JIT_CACHE result
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::JITCachedFunctionSearchFinished, functionId, result));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::JITFunctionPitched(
    _In_ FunctionID functionId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::JITFunctionPitched, functionId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::JITInlining(
    _In_ FunctionID callerId,
    _In_ FunctionID calleeId,
    /* [out] */ BOOL *pfShouldInline
    )
{
    HRESULT hr = S_OK;
    *pfShouldInline = FALSE;

    PROF_CALLBACK_BEGIN

    CCriticalSectionHolder holder(&m_csForJIT);

    if (m_attachedClrVersion != ClrVersion_2)
    {
        BOOL bShouldInline = FALSE;

        CComPtr<CMethodInfo> pInlineeMethodInfo;
        hr = CreateMethodInfo(calleeId, &pInlineeMethodInfo);
        if (FAILED(hr))
        {
            // No method info. Probably a dynamic module. The engine cannot instrument dynamic code.
            *pfShouldInline = TRUE;
            return S_OK;
        }
        CCleanupMethodInfo inlineeMethodInfoCleanup(pInlineeMethodInfo);

        mdToken inlineeToken;
        IfFailRet(pInlineeMethodInfo->GetMethodToken(&inlineeToken));

        CComPtr<CMethodInfo> pInlineSiteMethodInfo;
        CCleanupMethodInfo inlineSiteMethodInfoCleanup;

        if (calleeId != callerId)
        {
            hr = CreateMethodInfo(callerId, &pInlineSiteMethodInfo);
            if (FAILED(hr) || pInlineSiteMethodInfo == nullptr)
            {
                // No method info. Probably a dynamic module. The engine cannot instrument dynamic code.
                *pfShouldInline = TRUE;
                return S_OK;
            }

            IfFailRet(inlineSiteMethodInfoCleanup.SetMethodInfo(pInlineSiteMethodInfo));
        }
        else
        {
            // method inlined into itself. Don't enable inlineSiteMethodInfoCleanup or this will be a double delete
            pInlineSiteMethodInfo = pInlineeMethodInfo;
        }


        mdToken inlineSiteToken;
        IfFailRet(pInlineSiteMethodInfo->GetMethodToken(&inlineSiteToken));

        // Instrumentation engine asks each instrumentation method if inlining should be allowed for this function
        IfFailRet(CallAllowInlineOnInstrumentationMethods(pInlineeMethodInfo, pInlineSiteMethodInfo, &bShouldInline));

        if (bShouldInline)
        {
            // instrumentation methods said to allow inlining. Try the raw callback
            IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::JITInlining, callerId, calleeId, &bShouldInline));
        }

        CComPtr<IModuleInfo> pInlineeModuleInfo;
        IfFailRet(pInlineeMethodInfo->GetModuleInfo(&pInlineeModuleInfo));

        CComPtr<IModuleInfo> pInlineSiteModuleInfo;
        IfFailRet(pInlineSiteMethodInfo->GetModuleInfo(&pInlineSiteModuleInfo));

        CComPtr<CInlineSiteMap> pInlineSiteMap;
        IfFailRet(((CModuleInfo*)pInlineeModuleInfo.p)->GetInlineSiteMap(&pInlineSiteMap));

        if (bShouldInline)
        {
            // callbacks said to allow inlining for this function.
            // Maintain map of caller to callee. If a rejit occurs on callee,
            // also rejit caller. Note that caller may itself be inlined, so rejit it as well.
            IfFailRet(pInlineSiteMap->AddInlineSite(inlineeToken, inlineSiteToken, pInlineSiteModuleInfo));
        }
        else
        {
            // Do not remove the inline site entry. Doing so means that the methods the inline were removed
            // from will never rejit and therefore the inline site never comes back. If a method is rejitted
            // that contains inline sites, those sites should be rejitted when the method in question rejits.
        }

        *pfShouldInline = bShouldInline;
    }
    else
    {
        IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::JITInlining, callerId, calleeId, pfShouldInline));
    }

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ThreadCreated(
    _In_ ThreadID threadId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ThreadCreated, threadId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ThreadDestroyed(
    _In_ ThreadID threadId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ThreadDestroyed, threadId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ThreadAssignedToOSThread(
    _In_ ThreadID managedThreadId,
    _In_ DWORD osThreadId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ThreadAssignedToOSThread, managedThreadId, osThreadId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RemotingClientInvocationStarted()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RemotingClientInvocationStarted));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RemotingClientSendingMessage(
    _In_ GUID *pCookie,
    _In_ BOOL fIsAsync
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RemotingClientSendingMessage, pCookie, fIsAsync));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RemotingClientReceivingReply(
    _In_ GUID *pCookie,
    _In_ BOOL fIsAsync
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RemotingClientReceivingReply, pCookie, fIsAsync));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RemotingClientInvocationFinished()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RemotingClientInvocationFinished));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RemotingServerReceivingMessage(
    _In_ GUID *pCookie,
    _In_ BOOL fIsAsync
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RemotingServerReceivingMessage, pCookie, fIsAsync));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RemotingServerInvocationStarted()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RemotingServerInvocationStarted));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RemotingServerInvocationReturned()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RemotingServerInvocationReturned));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RemotingServerSendingReply(
    _In_ GUID *pCookie,
    _In_ BOOL fIsAsync
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RemotingServerSendingReply, pCookie, fIsAsync));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::UnmanagedToManagedTransition(
    _In_ FunctionID functionId,
    _In_ COR_PRF_TRANSITION_REASON reason
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::UnmanagedToManagedTransition, functionId, reason));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ManagedToUnmanagedTransition(
    _In_ FunctionID functionId,
    _In_ COR_PRF_TRANSITION_REASON reason
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ManagedToUnmanagedTransition, functionId, reason));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RuntimeSuspendStarted(
    _In_ COR_PRF_SUSPEND_REASON suspendReason
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RuntimeSuspendStarted, suspendReason));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RuntimeSuspendFinished()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RuntimeSuspendFinished));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RuntimeSuspendAborted()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RuntimeSuspendAborted));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RuntimeResumeStarted()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RuntimeResumeStarted));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RuntimeResumeFinished()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RuntimeResumeFinished));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RuntimeThreadSuspended(
    _In_ ThreadID threadId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RuntimeThreadSuspended, threadId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RuntimeThreadResumed(
    _In_ ThreadID threadId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RuntimeThreadResumed, threadId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::MovedReferences(
    _In_ ULONG cMovedObjectIDRanges,
    _In_reads_(cMovedObjectIDRanges) ObjectID oldObjectIDRangeStart[],
    _In_reads_(cMovedObjectIDRanges) ObjectID newObjectIDRangeStart[],
    _In_reads_(cMovedObjectIDRanges) ULONG cObjectIDRangeLength[]
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::MovedReferences, cMovedObjectIDRanges, oldObjectIDRangeStart, newObjectIDRangeStart, cObjectIDRangeLength));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ObjectAllocated(
    _In_ ObjectID objectId,
    _In_ ClassID classId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ObjectAllocated, objectId, classId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ObjectsAllocatedByClass(
    _In_ ULONG cClassCount,
    _In_reads_(cClassCount) ClassID classIds[],
    _In_reads_(cClassCount) ULONG cObjects[]
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ObjectsAllocatedByClass, cClassCount, classIds, cObjects));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ObjectReferences(
    _In_ ObjectID objectId,
    _In_ ClassID classId,
    _In_ ULONG cObjectRefs,
    _In_reads_(cObjectRefs) ObjectID objectRefIds[]
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ObjectReferences, objectId, classId, cObjectRefs, objectRefIds));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RootReferences(
    _In_ ULONG cRootRefs,
    _In_reads_(cRootRefs) ObjectID rootRefIds[]
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::RootReferences, cRootRefs, rootRefIds));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionThrown(
    _In_ ObjectID thrownObjectId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodExceptionEvents::ExceptionThrown, (UINT_PTR)thrownObjectId));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionThrown, thrownObjectId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionSearchFunctionEnter(
    _In_ FunctionID functionId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    CComPtr<CMethodInfo> pMethodInfo;
    IfFailRet(CreateNewMethodInfo(functionId, &pMethodInfo));

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodExceptionEvents::ExceptionSearchFunctionEnter, (IMethodInfo*)pMethodInfo));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionSearchFunctionEnter, functionId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionSearchFunctionLeave()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodExceptionEvents::ExceptionSearchFunctionLeave));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionSearchFunctionLeave));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionSearchFilterEnter(
    _In_ FunctionID functionId)
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    CComPtr<CMethodInfo> pMethodInfo;
    IfFailRet(CreateNewMethodInfo(functionId, &pMethodInfo));

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodExceptionEvents::ExceptionSearchFilterEnter, (IMethodInfo*)pMethodInfo));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionSearchFilterEnter, functionId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionSearchFilterLeave()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodExceptionEvents::ExceptionSearchFilterLeave));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionSearchFilterLeave));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionSearchCatcherFound(
    _In_ FunctionID functionId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    CComPtr<CMethodInfo> pMethodInfo;
    IfFailRet(CreateNewMethodInfo(functionId, &pMethodInfo));

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodExceptionEvents::ExceptionSearchCatcherFound, (IMethodInfo*)pMethodInfo));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionSearchCatcherFound, functionId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionOSHandlerEnter(
    _In_ UINT_PTR __unused
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionOSHandlerEnter, __unused));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionOSHandlerLeave(
    _In_ UINT_PTR __unused
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionOSHandlerLeave, __unused));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionUnwindFunctionEnter(
    _In_ FunctionID functionId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    CComPtr<CMethodInfo> pMethodInfo;
    IfFailRet(CreateNewMethodInfo(functionId, &pMethodInfo));

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodExceptionEvents::ExceptionUnwindFunctionEnter, (IMethodInfo*)pMethodInfo));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionUnwindFunctionEnter, functionId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionUnwindFunctionLeave()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodExceptionEvents::ExceptionUnwindFunctionLeave));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionUnwindFunctionLeave));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionUnwindFinallyEnter(
    _In_ FunctionID functionId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    CComPtr<CMethodInfo> pMethodInfo;
    IfFailRet(CreateNewMethodInfo(functionId, &pMethodInfo));

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodExceptionEvents::ExceptionUnwindFinallyEnter, (IMethodInfo*)pMethodInfo));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionUnwindFinallyEnter, functionId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionUnwindFinallyLeave()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodExceptionEvents::ExceptionUnwindFinallyLeave));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionUnwindFinallyLeave));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionCatcherEnter(
    _In_ FunctionID functionId,
    _In_ ObjectID objectId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    CComPtr<CMethodInfo> pMethodInfo;
    IfFailRet(CreateNewMethodInfo(functionId, &pMethodInfo));

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodExceptionEvents::ExceptionCatcherEnter, (IMethodInfo*)pMethodInfo, objectId));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionCatcherEnter, functionId, (ObjectID)objectId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionCatcherLeave()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodExceptionEvents::ExceptionCatcherLeave));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionCatcherLeave));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::COMClassicVTableCreated(
    _In_ ClassID wrappedClassId,
    _In_ REFGUID implementedIID,
    _In_ void *pVTable,
    _In_ ULONG cSlots
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    // Compiler complains that these callbacks taking void* parameters are ambiguous. Can't use variadic templates on this call.

    CComPtr<ICorProfilerCallback> pCallback;

    // Holding the lock during the callback functions is dangerous since rentrant events and calls will block.
    {
        CCriticalSectionHolder lock(&m_cs);

        if (m_profilerCallbackHolder != nullptr)
        {
            pCallback = (ICorProfilerCallback*)(m_profilerCallbackHolder->GetMemberForInterface(__uuidof(ICorProfilerCallback)));
        }
    }

    if (pCallback != nullptr)
    {
        hr = pCallback->COMClassicVTableCreated(wrappedClassId, implementedIID, pVTable, cSlots);
    }

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::COMClassicVTableDestroyed(
    _In_ ClassID wrappedClassId,
    _In_ REFGUID implementedIID,
    _In_ void *pVTable
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    // Compiler complains that these callbacks taking void* parameters are ambiguous. Can't use variadic templates on this call.
    CComPtr<ICorProfilerCallback> pCallback;

    // Holding the lock during the callback functions is dangerous since rentrant
    // events and calls will block. Copy the collection under the lock, then release it and finally call the callbacks
    {
        CCriticalSectionHolder lock(&m_cs);

        if (m_profilerCallbackHolder != nullptr)
        {
            pCallback = (ICorProfilerCallback*)(m_profilerCallbackHolder->GetMemberForInterface(__uuidof(ICorProfilerCallback)));
        }
    }

    if (pCallback != nullptr)
    {
        hr = pCallback->COMClassicVTableDestroyed(wrappedClassId, implementedIID, pVTable);
    }

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionCLRCatcherFound()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionCLRCatcherFound));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ExceptionCLRCatcherExecute()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback::ExceptionCLRCatcherExecute));

    PROF_CALLBACK_END

    return S_OK;
}

// ICorProfilerCallback2 methods
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ThreadNameChanged(
    _In_ ThreadID threadId,
    _In_ ULONG cchName,
    _In_reads_opt_(cchName) WCHAR name[]
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback2::ThreadNameChanged, threadId, cchName, name));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GarbageCollectionStarted(
    _In_ int cGenerations,
    _In_reads_(cGenerations) BOOL generationCollected[],
    _In_ COR_PRF_GC_REASON reason
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback2::GarbageCollectionStarted, cGenerations, generationCollected, reason));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::SurvivingReferences(
    _In_ ULONG cSurvivingObjectIDRanges,
    _In_reads_(cSurvivingObjectIDRanges) ObjectID objectIDRangeStart[],
    _In_reads_(cSurvivingObjectIDRanges) ULONG cObjectIDRangeLength[]
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback2::SurvivingReferences, cSurvivingObjectIDRanges, objectIDRangeStart, cObjectIDRangeLength));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GarbageCollectionFinished()
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback2::GarbageCollectionFinished));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::FinalizeableObjectQueued(
    _In_ DWORD finalizerFlags,
    _In_ ObjectID objectID
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback2::FinalizeableObjectQueued, finalizerFlags, objectID));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::RootReferences2(
    _In_ ULONG cRootRefs,
    _In_reads_(cRootRefs) ObjectID rootRefIds[],
    _In_reads_(cRootRefs) COR_PRF_GC_ROOT_KIND rootKinds[],
    _In_reads_(cRootRefs) COR_PRF_GC_ROOT_FLAGS rootFlags[],
    _In_reads_(cRootRefs) UINT_PTR rootIds[]
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback2::RootReferences2, cRootRefs, rootRefIds, rootKinds, rootFlags, rootIds));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::HandleCreated(
    _In_ GCHandleID handleId,
    _In_ ObjectID initialObjectId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback2::HandleCreated, handleId, initialObjectId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::HandleDestroyed(
    _In_ GCHandleID handleId
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback2::HandleDestroyed, handleId));

    PROF_CALLBACK_END

    return S_OK;
}

// ICorProfilerCallback3 methods
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::InitializeForAttach(
    _In_ IUnknown *pCorProfilerInfoUnk,
    _In_ void *pvClientData,
    _In_ UINT cbClientData
    )
{
    HRESULT hr = S_OK;

    // TODO: How to handle this? Does this need to support profiler attach? How does that impact initlization etc...

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback3::InitializeForAttach, pCorProfilerInfoUnk, pvClientData, cbClientData));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ProfilerAttachComplete(void)
{
    HRESULT hr = S_OK;

    // TODO: How to handle this? Does this need to support profiler attach? How does that impact initlization etc...

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback3::ProfilerAttachComplete));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ProfilerDetachSucceeded(void)
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback3::ProfilerDetachSucceeded));

    PROF_CALLBACK_END

    return S_OK;
}

// ICorProfilerCallback4 methods
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ReJITCompilationStarted(
    _In_ FunctionID functionId,
    _In_ ReJITID rejitId,
    _In_ BOOL fIsSafeToBlock
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToInstrumentationMethods(&IInstrumentationMethodJitEvents::JitStarted, functionId, TRUE));

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback4::ReJITCompilationStarted, functionId, rejitId, fIsSafeToBlock));

    PROF_CALLBACK_END

    return S_OK;
}

// This is the primary function by which the rejit il is obtained. Give the instrumentation methods first crack and then
// pass the il from there to here.
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetReJITParameters(
    _In_ ModuleID moduleId,
    _In_ mdMethodDef methodToken,
    _In_ ICorProfilerFunctionControl* pFunctionControl
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    if (m_attachedClrVersion != ClrVersion_2)
    {
        // Only allow a single method to be instrumented at a time.
        CCriticalSectionHolder holder(&m_csForJIT);

        CComPtr<CMethodInfo> pMethodInfo;
        IfFailRet(CreateMethodInfoForRejit(moduleId, methodToken, pFunctionControl, &pMethodInfo));
        ClearILTransformationStatus(moduleId, methodToken);

        // CreateMethodInfoForRejit adds the method info the token to method info map.
        // This class is used to cleanup that reference after the rejit has finished
        class CRemoveMethodInfoAfterRejit
        {
        private:
            CComPtr<CModuleInfo> m_pModuleInfo;
            mdToken m_methodToken;

        public:
            CRemoveMethodInfoAfterRejit(_In_ CModuleInfo* pModuleInfo, _In_ mdToken methodToken) : m_pModuleInfo(pModuleInfo), m_methodToken(methodToken)
            {
            }

            ~CRemoveMethodInfoAfterRejit()
            {
                if (m_pModuleInfo != nullptr)
                {
                    m_pModuleInfo->SetRejitMethodInfo(m_methodToken, nullptr);
                    m_pModuleInfo.Release();
                }
            }
        };
        CRemoveMethodInfoAfterRejit methodInfoRemover(pMethodInfo->GetModuleInfo(), methodToken);

        CComPtr<IModuleInfo> pModuleInfo;
        IfFailRet(pMethodInfo->GetModuleInfo(&pModuleInfo));

        BOOL isDynamic = false;
        IfFailRet(pModuleInfo->GetIsDynamic(&isDynamic));

        // It is not possible to instrument methods that don't have an image base.
        if (!isDynamic)
        {

            // Query if the instrumentation methods want to instrument and then have them actually instrument.
            vector<CComPtr<IInstrumentationMethod>> toInstrument;
            IfFailRet(CallShouldInstrumentOnInstrumentationMethods(pMethodInfo, TRUE, &toInstrument));

            // Give the instrumentation methods a chance to do method body replacement. Only one can replace the
            // method body
            IfFailRet(CallBeforeInstrumentMethodOnInstrumentationMethods(pMethodInfo, TRUE, toInstrument));

            // Instrumentation methods to most of their instrumentation during InstrumentMethod
            IfFailRet(CallInstrumentOnInstrumentationMethods(pMethodInfo, TRUE, toInstrument));

            CComPtr<CCorProfilerFunctionInfoWrapper> pWrappedCorProfilerFunctionControl;
            pWrappedCorProfilerFunctionControl.Attach(new CCorProfilerFunctionInfoWrapper(this, pMethodInfo));

            if (!pWrappedCorProfilerFunctionControl)
            {
                return E_OUTOFMEMORY;
            }

            // Send the event to the raw callbacks to give them a chance to instrument.
            IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback4::GetReJITParameters, moduleId, methodToken, (ICorProfilerFunctionControl*)(pWrappedCorProfilerFunctionControl.p)));

            // If the method was instrumetned
            if (pMethodInfo->IsInstrumented())
            {
                // Give the final instrumentation to the clr.
                IfFailRet(pMethodInfo->ApplyFinalInstrumentation());

                // Don't fail out with this call. It is too late to undo anything with a failure.
                CallOnInstrumentationComplete(pMethodInfo, true);
            }
        }

        // Don't call cleanup on method info for rejit as we don't have the function id and the methodinfo only needs to survive for the
        // duration of this call
        // IfFailRet(pMethodInfo->Cleanup());
    }
    else
    {
        // Send the event to the raw callbacks to give them a chance to instrument.
        IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback4::GetReJITParameters, moduleId, methodToken, pFunctionControl));
    }

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ReJITCompilationFinished(
    _In_ FunctionID functionId,
    _In_ ReJITID rejitId,
    _In_ HRESULT hrStatus,
    _In_ BOOL fIsSafeToBlock
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IGNORE_IN_NET20_BEGIN

    CComPtr<CMethodJitInfo> pMethodJitInfo;
    pMethodJitInfo.Attach(new CMethodJitInfo(functionId, hrStatus, true, rejitId, this));
    ULONG32 cIlMap = 0;

    // for some versions of the clr, the ICorFunctionInfo for the method does not correctly update the
    // il maps. Attempt to set them after rejit completes using the profiler info.
    // delmyers TODO: there is a fix planned for this with core clr 3. At that point, we may
    // consider optimizing so that we don't force extra reading/writing of the cache.

    if (SUCCEEDED(pMethodJitInfo->GetILInstrumentationMap(0, nullptr, &cIlMap)))
    {
        unique_ptr<COR_IL_MAP[]> ilMap = make_unique<COR_IL_MAP[]>(cIlMap);
        if (SUCCEEDED(pMethodJitInfo->GetILInstrumentationMap(cIlMap, ilMap.get(), &cIlMap)))
        {
            m_pRealProfilerInfo->SetILInstrumentedCodeMap(functionId, FALSE, cIlMap, ilMap.get());

            // force a read from the pMethodJitInfo. This will cause the runtime to cache
            // the values correctly.
            ULONG32 cTemp = 0;
            pMethodJitInfo->GetILNativeMapping(0, NULL, &cTemp);
        }
    }
    IfFailRet(ForEachInstrumentationMethod(Events::SendJitCompleteEvent, pMethodJitInfo));

    IGNORE_IN_NET20_END

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback4::ReJITCompilationFinished, functionId, rejitId, hrStatus, fIsSafeToBlock));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ReJITError(
    _In_ ModuleID moduleId,
    _In_ mdMethodDef methodId,
    _In_ FunctionID functionId,
    _In_ HRESULT hrStatus
    )
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback4::ReJITError, moduleId, methodId, functionId, hrStatus));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::MovedReferences2(
    _In_ ULONG cMovedObjectIDRanges,
    _In_reads_(cMovedObjectIDRanges) ObjectID oldObjectIDRangeStart[],
    _In_reads_(cMovedObjectIDRanges) ObjectID newObjectIDRangeStart[],
    _In_reads_(cMovedObjectIDRanges) SIZE_T cObjectIDRangeLength[]
)
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback4::MovedReferences2, cMovedObjectIDRanges, oldObjectIDRangeStart, newObjectIDRangeStart, cObjectIDRangeLength));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::SurvivingReferences2(
    _In_ ULONG cSurvivingObjectIDRanges,
    _In_reads_(cSurvivingObjectIDRanges) ObjectID objectIDRangeStart[],
    _In_reads_(cSurvivingObjectIDRanges) SIZE_T cObjectIDRangeLength[]
)
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback4::SurvivingReferences2, cSurvivingObjectIDRanges, objectIDRangeStart, cObjectIDRangeLength));

    PROF_CALLBACK_END

    return S_OK;
}

// ICorProfilerCallback5 methods
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ConditionalWeakTableElementReferences(
    _In_ ULONG cRootRefs,
    _In_reads_(cRootRefs) ObjectID keyRefIds[],
    _In_reads_(cRootRefs) ObjectID valueRefIds[],
    _In_reads_(cRootRefs) GCHandleID rootIds[]
)
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback5::ConditionalWeakTableElementReferences, cRootRefs, keyRefIds, valueRefIds, rootIds));

    PROF_CALLBACK_END

    return S_OK;
}

// ICorProfilerCallback6 methods
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetAssemblyReferences(
    _In_ const WCHAR *wszAssemblyPath,
    _In_ ICorProfilerAssemblyReferenceProvider *pAsmRefProvider)
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback6::GetAssemblyReferences, wszAssemblyPath, pAsmRefProvider));

    PROF_CALLBACK_END

    return S_OK;
}

//ICorProfilerCallback7 methods
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ModuleInMemorySymbolsUpdated(
    _In_ ModuleID moduleId)
{
    HRESULT hr = S_OK;

    PROF_CALLBACK_BEGIN

    IfFailRet(SendEventToRawProfilerCallback(&ICorProfilerCallback7::ModuleInMemorySymbolsUpdated, moduleId));

    PROF_CALLBACK_END

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ConstructAppDomainInfo(
    _In_ AppDomainID appDomainId,
    _Out_ IAppDomainInfo** ppAppDomainInfo
    )
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppAppDomainInfo);

    *ppAppDomainInfo = NULL;

    // The CLR will send ModuleLoadFinished callback before AppDomainCreationFinished
    // This ensures the CAppDomainInfo is in place to process such sequence of events.
    // The AppDomainInfoCollection knows how to deal with duplicate addition calls.
    // Note that this means the collection may contain incomplete objects
    CComPtr<CAppDomainInfo> pAppDomainInfo;
    pAppDomainInfo.Attach(new CAppDomainInfo(appDomainId));
    if (pAppDomainInfo == NULL)
    {
        CLogging::LogError(_T("Failed to construct appdomain info."));
        return E_FAIL;
    }

    IfFailRet(m_pAppDomainCollection->AddAppDomainInfo(appDomainId, pAppDomainInfo));

    *ppAppDomainInfo = pAppDomainInfo.Detach();

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ConstructAssemblyInfo(_In_ AssemblyID assemblyId, _Out_ IAssemblyInfo** ppAssemblInfo)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppAssemblInfo);

    *ppAssemblInfo = NULL;

    ULONG cchAssemblyName = 0;
    IfFailRet(m_pRealProfilerInfo->GetAssemblyInfo(assemblyId, 0, &cchAssemblyName, nullptr, nullptr, nullptr));

    CAutoVectorPtr<WCHAR> wszAssemblyName(new WCHAR[cchAssemblyName]);
    if (wszAssemblyName == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    AppDomainID appDomainID = 0;
    ModuleID manifestModuleID = 0;
    IfFailRet(m_pRealProfilerInfo->GetAssemblyInfo(assemblyId, cchAssemblyName, &cchAssemblyName, wszAssemblyName, &appDomainID, &manifestModuleID));

    CComPtr<IAppDomainInfo> pAppDomainInfo;
    IfFailRet(m_pAppDomainCollection->GetAppDomainById(appDomainID, &pAppDomainInfo));


    CComPtr<CAssemblyInfo> pAssemblyInfo;
    pAssemblyInfo.Attach(new CAssemblyInfo());
    if (pAssemblyInfo == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    IfFailRet(pAssemblyInfo->Initialize(
        assemblyId,
        wszAssemblyName.m_p,
        pAppDomainInfo,
        manifestModuleID
        ));

    CAppDomainInfo* pRawAppDomainInfo = static_cast<CAppDomainInfo*>(pAppDomainInfo.p);
    IfFailRet(pRawAppDomainInfo->AddAssemblyInfo(pAssemblyInfo));

    *ppAssemblInfo = pAssemblyInfo.Detach();

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ConstructModuleInfo(
    _In_ ModuleID moduleId,
    _Out_ IModuleInfo** ppModuleInfo
    )
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppModuleInfo);
    *ppModuleInfo = NULL;

    ULONG cchModulePath = 0;
    IfFailRet(m_pRealProfilerInfo->GetModuleInfo(moduleId, nullptr, 0, &cchModulePath, nullptr, nullptr));

    CAutoVectorPtr<WCHAR> wszModulePath(new WCHAR[cchModulePath]);
    if (wszModulePath == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    LPCBYTE pModuleBaseLoadAddress = nullptr;
    AssemblyID assemblyId;
    IfFailRet(m_pRealProfilerInfo->GetModuleInfo(moduleId, &pModuleBaseLoadAddress, cchModulePath, &cchModulePath, wszModulePath, &assemblyId));

    CComPtr<IMetaDataAssemblyImport> pMetadataAssemblyImport;
    hr = m_pRealProfilerInfo->GetModuleMetaData(moduleId, ofRead, IID_IMetaDataAssemblyImport, (IUnknown**)&pMetadataAssemblyImport);
    if (FAILED(hr) || hr == S_FALSE)
    {
        // If this module is resource only, don't continue to construct a module info.
        CLogging::LogMessage(_T("%s is a metadata only assembly and will not be tracked with a module info instance."), wszModulePath.m_p);
        return E_FAIL;
    }

    CComPtr<IMetaDataImport2> pMetadataImport;
    IfFailRet(m_pRealProfilerInfo->GetModuleMetaData(moduleId, ofRead, IID_IMetaDataImport2, (IUnknown**)&pMetadataImport));


    // Always get the emit interfaces if we can emit, since we may need these later on.  We
    // must do this *now* since the CLR will prevent us from calling GetModuleMetaData unless we're in a
    // profiler callback.  DumpObject might use these interfaces since it emits typespecs when dumping generic
    // objects.  NOTE: this eats up large amounts of memory, possibly needlessly.  A long term fix would be to
    // change any code that uses Emit to use Import first to find the existing type specs; there's a good chance
    // we don't need to emit typespecs at all.
    //
    // NOTE: these calls will fail for winmd. This is okay
    CComPtr<IMetaDataEmit2> pMetaDataEmit2;
    CComPtr<IMetaDataAssemblyEmit> pMetaDataAssemblyEmit;
    m_pRealProfilerInfo->GetModuleMetaData (moduleId, ofRead | ofWrite, IID_IMetaDataEmit2, (IUnknown**)&pMetaDataEmit2);
    m_pRealProfilerInfo->GetModuleMetaData (moduleId, ofRead | ofWrite, IID_IMetaDataAssemblyEmit, (IUnknown**)&pMetaDataAssemblyEmit);

    CComPtr<CModuleInfo> pModuleInfo;
    pModuleInfo.Attach(new CModuleInfo);
    if (pModuleInfo == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    AppDomainID appdomainId = 0;
    IfFailRet(m_pRealProfilerInfo->GetAssemblyInfo(assemblyId, 0, nullptr, nullptr, &appdomainId, nullptr));


    CComPtr<IAppDomainInfo> pAppDomainInfo;
    hr = m_pAppDomainCollection->GetAppDomainById(appdomainId, &pAppDomainInfo);
    if (FAILED(hr))
    {
        IfFailRet(ConstructAppDomainInfo(appdomainId, &pAppDomainInfo));
    }


    CComPtr<IAssemblyInfo> pAssemblyInfo;
    hr = m_pAppDomainCollection->GetAssemblyInfoById(assemblyId, &pAssemblyInfo);
    if (FAILED(hr))
    {
        // No assemblyinfo created yet, so create one.
        // NOTE: This is very common as modules often load before the assembly load finishes callback
        IfFailRet(ConstructAssemblyInfo(assemblyId, &pAssemblyInfo));
    }

    IfFailRet(pModuleInfo->Initialize(
        moduleId,
        wszModulePath.m_p,
        pAssemblyInfo,
        pAppDomainInfo,
        pModuleBaseLoadAddress,
        pMetadataImport,
        pMetadataAssemblyImport,
        pMetaDataEmit2,
        pMetaDataAssemblyEmit
        ));

    IfFailRet(static_cast<CAppDomainInfo*>(pAppDomainInfo.p)->AddModuleInfo(pModuleInfo));
    IfFailRet(static_cast<CAssemblyInfo*>(pAssemblyInfo.p)->AddModuleInfo(pModuleInfo));

    *ppModuleInfo = pModuleInfo.Detach();

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::CreateMethodInfo(_In_ FunctionID functionId, _Out_ CMethodInfo** ppMethodInfo)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CProfilerManager::CreateMethodInfo"));

    IfNullRetPointer(ppMethodInfo);
    *ppMethodInfo = NULL;

    ClassID classId;
    ModuleID moduleId;
    mdToken functionToken;
    IfFailRet(m_pRealProfilerInfo->GetFunctionInfo(functionId, &classId, &moduleId, &functionToken));

    CComPtr<CModuleInfo> pModuleInfo;
    hr = m_pAppDomainCollection->GetModuleInfoById(moduleId, (IModuleInfo**)&pModuleInfo);
    if (FAILED(hr))
    {
        CLogging::LogMessage(_T("CProfilerManager::CreateMethodInfo - no moduleinfo found. Probably a dynamic module %x"), moduleId);
        return E_FAIL;
    }

    // Check if a method info already exists for this function id.
    CComPtr<CMethodInfo> pMethodInfo;
    hr = pModuleInfo->GetMethodInfoById(functionId, &pMethodInfo);
    if (SUCCEEDED(hr))
    {
        // A method info already existed. It must have leaked in the collections, as both callers expect new ones to be created.
        // Log an error and overwrite the collection.
        CComBSTR bstrMethodFullName;
        IfFailRet(pMethodInfo->GetFullName(&bstrMethodFullName));

        mdToken token;
        IfFailRet(pMethodInfo->GetMethodToken(&token));

        CComPtr<IModuleInfo> pExistingModuleInfo;
        IfFailRet(pMethodInfo->GetModuleInfo(&pExistingModuleInfo));

        ModuleID existingModuleId;
        IfFailRet(pExistingModuleInfo->GetModuleID(&existingModuleId));

        CLogging::LogError(_T("CProfilerManager::CreateMethodInfo - A methodinfo already existed for this function id/module. This means one must have leaked. FunctionId:0x%x, ModuleId:0x%x, FullName:%s, MethodTokenExistingMethodInfo:0x%x, MethodTokenNewMethodInfo:0x%x, ExistingModuleId:0x%x"),
            functionId,
            moduleId,
            bstrMethodFullName.m_str,
            token,
            functionToken,
            existingModuleId
        );
    }


    CLogging::LogMessage(_T("CProfilerManager::CreateMethodInfo - creating new method info"));
    pMethodInfo.Attach(new CMethodInfo(functionId, functionToken, classId, pModuleInfo, NULL));

    IfFailRet(pMethodInfo->Initialize(true, false));

    IfFailRet(pModuleInfo->AddMethodInfo(functionId, pMethodInfo));

    *ppMethodInfo = pMethodInfo.Detach();

    CLogging::LogMessage(_T("End CProfilerManager::CreateMethodInfo"));

    return S_OK;
}

// This creates a new methodinfo that is not shared with the instrumentation code paths.
// This ensures isolation of lifetimes for the cases where reobtaining the methodinfo is not necessary
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::CreateNewMethodInfo(_In_ FunctionID functionId, _Out_ CMethodInfo** ppMethodInfo)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CProfilerManager::CreateNewMethodInfo"));

    IfNullRetPointer(ppMethodInfo);
    *ppMethodInfo = NULL;

    ClassID classId;
    ModuleID moduleId;
    mdToken functionToken;
    IfFailRet(m_pRealProfilerInfo->GetFunctionInfo(functionId, &classId, &moduleId, &functionToken));

    CComPtr<CModuleInfo> pModuleInfo;
    hr = m_pAppDomainCollection->GetModuleInfoById(moduleId, (IModuleInfo**)&pModuleInfo);
    if (FAILED(hr))
    {
        CLogging::LogMessage(_T("CProfilerManager::CreateNewMethodInfo - no method info found. Probably a dynamic module %x"), moduleId);
        return E_FAIL;
    }

    CComPtr<CMethodInfo> pMethodInfo;
    CLogging::LogMessage(_T("CProfilerManager::CreateNewMethodInfo - creating new method info"));
    pMethodInfo.Attach(new CMethodInfo(functionId, functionToken, classId, pModuleInfo, NULL));

    IfFailRet(pMethodInfo->Initialize(false, false));

    *ppMethodInfo = pMethodInfo.Detach();

    CLogging::LogMessage(_T("End CProfilerManager::CreateNewMethodInfo"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::CreateMethodInfoForRejit(
    _In_ ModuleID moduleId,
    _In_ mdMethodDef methodToken,
    _In_ ICorProfilerFunctionControl* pFunctionControl,
    _Out_ CMethodInfo** ppMethodInfo
    )
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CProfilerManager::CreateMethodInfoForRejit"));

    IfNullRetPointer(ppMethodInfo);
    *ppMethodInfo = NULL;

    CComPtr<CModuleInfo> pModuleInfo;
    IfFailRet(m_pAppDomainCollection->GetModuleInfoById(moduleId, (IModuleInfo**)&pModuleInfo));

    CLogging::LogMessage(_T("CProfilerManager::CreateMethodInfoForRejit - creating new method info"));
    CComPtr<CMethodInfo> pMethodInfo;
    pMethodInfo.Attach(new CMethodInfo(0, methodToken, 0, pModuleInfo, pFunctionControl));

    IfFailRet(pMethodInfo->Initialize(false, true));

    IfFailRet(pModuleInfo->SetRejitMethodInfo(methodToken, pMethodInfo));

    *ppMethodInfo = pMethodInfo.Detach();

    CLogging::LogMessage(_T("End CProfilerManager::CreateMethodInfoForRejit"));

    return S_OK;
}

// Call BeforeInstrumentMethod on each instrumentation method. This gives them a chance to replace the method
// body using CreateBaseline. Note that only one instrumentation method will be allowed to replace the method body.
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::CallBeforeInstrumentMethodOnInstrumentationMethods(
    _In_ IMethodInfo* pMethodInfo,
    _In_ BOOL isRejit,
    _In_ vector<CComPtr<IInstrumentationMethod>>& toInstrument)
{
    HRESULT hr = S_OK;

    if (toInstrument.size() > 0)
    {
        for (CComPtr<IInstrumentationMethod> pCurrInstrumentationMethod : toInstrument)
        {
            CLogging::LogMessage(_T("Asking Instrumentation Method to instrument"));

            IfFailRet(pCurrInstrumentationMethod->BeforeInstrumentMethod(pMethodInfo, isRejit));
        }
    }

    ((CMethodInfo*)pMethodInfo)->DisableCreateBaseline();

    return S_OK;
}

// Call InstrumentMethod on each instrumentation method.
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::CallInstrumentOnInstrumentationMethods(
    _In_ IMethodInfo* pMethodInfo,
    _In_ BOOL isRejit,
    _In_ vector<CComPtr<IInstrumentationMethod>>& toInstrument)
{
    HRESULT hr = S_OK;

    if (toInstrument.size() > 0)
    {
        for (CComPtr<IInstrumentationMethod> pCurrInstrumentationMethod : toInstrument)
        {
            CLogging::LogMessage(_T("Asking Instrumentation Method to instrument"));

            if (CLogging::AllowLogEntry(LoggingFlags_InstrumentationResults))
            {
                if (!isRejit)
                {
                    FunctionID functionId = 0;
                    ((CMethodInfo*)pMethodInfo)->GetFunctionId(&functionId);

                    ClassID classId;
                    ModuleID moduleId;
                    mdToken functionToken;
                    IfFailRet(m_pRealProfilerInfo->GetFunctionInfo(functionId, &classId, &moduleId, &functionToken));

                    // Get ModuleInfo

                    ULONG cchModulePath = 0;
                    IfFailRet(m_pRealProfilerInfo->GetModuleInfo(moduleId, nullptr, 0, &cchModulePath, nullptr, nullptr));

                    std::vector<WCHAR> modulePath(cchModulePath);
                    IfFailRet(m_pRealProfilerInfo->GetModuleInfo(moduleId, nullptr, cchModulePath, &cchModulePath, modulePath.data(), nullptr));

                    CLogging::LogDumpMessage(_T("[TestIgnore]CProfilerManager::CallInstrumentOnInstrumentationMethods [JIT] for Module: %s\r\n"), modulePath.data());

                    // Get MethodInfo

                    CComPtr<IMetaDataImport2> pMetadataImport;
                    IfFailRet(m_pRealProfilerInfo->GetModuleMetaData(moduleId, ofRead, IID_IMetaDataImport, (IUnknown**)&pMetadataImport));

                    DWORD cbMethodName;
                    IfFailRet(pMetadataImport->GetMethodProps(functionToken, nullptr, nullptr, 0, &cbMethodName, nullptr, nullptr, nullptr, nullptr, nullptr));

                    std::vector<WCHAR> methodName(cbMethodName);
                    ULONG rva;
                    IfFailRet(pMetadataImport->GetMethodProps(functionToken, nullptr, methodName.data(), cbMethodName, &cbMethodName, nullptr, nullptr, nullptr, &rva, nullptr));

                    CLogging::LogDumpMessage(_T("[TestIgnore]   Method: %s, rva 0x%08x\r\n"), methodName.data(), rva);
                }
                else
                {
                    CComBSTR bstrMethodFullName;
                    ((CMethodInfo*)pMethodInfo)->GetFullName(&bstrMethodFullName);

                    ULONG rva;
                    ((CMethodInfo*)pMethodInfo)->GetCodeRva(&rva);

                    CLogging::LogDumpMessage(_T("[TestIgnore]CProfilerManager::CallInstrumentOnInstrumentationMethods [REJIT] for %s with rva 0x%08x\r\n"), bstrMethodFullName.m_str, rva);
                }
            }

            hr = pCurrInstrumentationMethod->InstrumentMethod(pMethodInfo, isRejit);

            CLogging::LogMessage(_T("Instrumentation Method finished instrumenting. Result hr is:. hr=%04x"), hr);

            // If any instrumentation method fails to instrument, stop instrumenting. This will cause the unchanged il to be used.
            IfFailRet(hr);
        }

        // After the instrumentation methods have done their instrumentation, render the result IL, etc...
        // This is done before the raw callbacks get a shot at instrumentation. The cor profiler wrapper
        // will give these results to callers when queried.
        IfFailRet(((CMethodInfo*)pMethodInfo)->ApplyIntermediateMethodInstrumentation());
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::CreateSignatureBuilder(_Out_ ISignatureBuilder ** ppSignatureBuilder)
{
    IfNullRetPointer(ppSignatureBuilder);
    *ppSignatureBuilder = nullptr;

    CComPtr<ISignatureBuilder> pSignatureBuilder;
    pSignatureBuilder.Attach(new CSignatureBuilder());
    if (pSignatureBuilder == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    *ppSignatureBuilder = pSignatureBuilder.Detach();
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::GetInstrumentationMethod(_In_ REFGUID cslid, _Out_ IUnknown** ppUnknown)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Start CProfilerManager::GetInstrumentationMethod"));
    IfNullRetPointer(ppUnknown);
    *ppUnknown = nullptr;

    CCriticalSectionHolder holder(&m_cs);

    // Linear search the instrumentation methods. This should be called very rarely, so an index
    // will likely just add memory overhead.
    TInstrumentationMethodsCollection::const_iterator it;
    for (it = m_instrumentationMethods.begin(); it != m_instrumentationMethods.end(); ++it)
    {
        if ((*it).first->GetClassId() == cslid)
        {
            CComPtr<IInstrumentationMethod> pRawInstrumentationMethod;
            IfFailRet((*it).first->GetRawInstrumentationMethod(&pRawInstrumentationMethod));

            IfFailRet(pRawInstrumentationMethod->QueryInterface(__uuidof(IUnknown), reinterpret_cast<LPVOID*>(ppUnknown)));
            CLogging::LogMessage(_T("End CProfilerManager::GetInstrumentationMethod"));
            return S_OK;
        }
    }

    CLogging::LogMessage(_T("End CProfilerManager::GetInstrumentationMethod"));

    return E_NOINTERFACE;
}



// Call ShouldInstrument on each instrumentation method. Return those that return true in pToInstrument
HRESULT MicrosoftInstrumentationEngine::CProfilerManager::CallShouldInstrumentOnInstrumentationMethods(
    _In_ IMethodInfo* pMethodInfo,
    _In_ BOOL isRejit,
    _Inout_ vector<CComPtr<IInstrumentationMethod>>* pToInstrument
    )
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Start CProfilerManager::CallShouldInstrumentOnInstrumentationMethods"));

    vector<CComPtr<IInstrumentationMethod>> instrumentMethodVector;
    IfFailRet(CopyInstrumentationMethods(instrumentMethodVector));

    // Send event to instrumentation methods
    for (CComPtr<IInstrumentationMethod> pCurrInstrumentationMethod : instrumentMethodVector)
    {
        CLogging::LogMessage(_T("Calling ShouldInstrumentMethod Instrumentation Method"));

        BOOL bShouldInstrument = FALSE;
        hr = pCurrInstrumentationMethod->ShouldInstrumentMethod(pMethodInfo, isRejit, &bShouldInstrument);

        CLogging::LogMessage(_T("ShouldInstrumentMethod returned value %04x. hr=%04x"), bShouldInstrument, hr);
        if (bShouldInstrument)
        {
            pToInstrument->push_back(pCurrInstrumentationMethod);
        }
    }

    CLogging::LogMessage(_T("End CProfilerManager::CallShouldInstrumentOnInstrumentationMethods"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::CallOnInstrumentationComplete(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Start CProfilerManager::CallOnInstrumentationComplete"));

    CCriticalSectionHolder lock(&m_cs);

    TInstrumentationMethodsCollection::const_iterator it;
    for (it = m_instrumentationMethods.begin(); it != m_instrumentationMethods.end(); ++it)
    {
        CComPtr<IInstrumentationMethod> pRawInstrumentationMethod;
        IfFailRet((*it).first->GetRawInstrumentationMethod(&pRawInstrumentationMethod));

        hr = pRawInstrumentationMethod->OnInstrumentationComplete(pMethodInfo, isRejit);
        if (FAILED(hr))
        {
            CLogging::LogError(_T("CProfilerManager::CallOnInstrumentationComplete - instrumentation method failed for CallOnInstrumentationComplete"));
        }
    }

    CLogging::LogMessage(_T("End CProfilerManager::CallOnInstrumentationComplete"));
    return hr;
}

// function called when a profiler callback swallows an exception to send the error report to watson as if the app crashed.
// Catching SEH exceptions is necessary because allowing the exception to escape the profiler will likely invoke an exception handler in MMA
// which will likely deadlock against the jit compilers critical section.
void MicrosoftInstrumentationEngine::SendErrorReport(EXCEPTION_POINTERS* pException)
{
#ifndef PLATFORM_UNIX
    // sergeyni: commented out because it's not compatible with WINVER 0x0600
    // RaiseFailFastException(pException->ExceptionRecord, pException->ContextRecord, 0);

    // replicating semantic of RaiseFailFastException
    // dwMode[in]
    //    This parameter is reserved for system use and should be set to zero.

    // wiktork We no longer ReportFault due to stricter privacy requirements for Watson dumps in Azure
    //::ReportFault(pException, 0);
    ::ExitProcess(pException->ExceptionRecord->ExceptionCode);
#endif
}

// Translator function that is called by the crt when an SEH exception happens so it can be converted into
// a C++ exception. This is done so C++ functions can invoke watson on seh exceptions. See documentation
// of _set_se_translator on msdn.
void __cdecl MicrosoftInstrumentationEngine::SehTranslatorFunc(unsigned int u, EXCEPTION_POINTERS* pExp)
{
    throw CSehException(pExp);
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::CallAllowInlineOnInstrumentationMethods(
    _In_ IMethodInfo* pInlineeMethodInfo,
    _In_ IMethodInfo* pInlineSiteMethodInfo,
    _Out_ BOOL* pbShouldInline
    )
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Start CProfilerManager::CallAllowInlineOnInstrumentationMethods"));

    vector<CComPtr<IInstrumentationMethod>> methods;
    IfFailRet(CopyInstrumentationMethods(methods));

    BOOL bShouldAllowInline = TRUE;
    for (CComPtr<IInstrumentationMethod> pRawInstrumentationMethod : methods)
    {
        hr = pRawInstrumentationMethod->AllowInlineSite(pInlineeMethodInfo, pInlineSiteMethodInfo, &bShouldAllowInline);
        if (FAILED(hr))
        {
            CLogging::LogError(_T("CProfilerManager::CallAllowInlineOnInstrumentationMethods - instrumentation method failed for CallAllowInlineOnInstrumentationMethods"));
        }

        if (!bShouldAllowInline)
        {
            // Once one says to disable inlining, we're done
            break;
        }
    }

    *pbShouldInline = bShouldAllowInline;

    CLogging::LogMessage(_T("End CProfilerManager::CallAllowInlineOnInstrumentationMethods"));
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ClearILTransformationStatus(_In_ FunctionID functionId)
{
    HRESULT hr;
    ClassID classId;
    ModuleID moduleId;
    mdMethodDef token;
    IfFailRet(m_pRealProfilerInfo->GetFunctionInfo(functionId, &classId, &moduleId, &token));
    return ClearILTransformationStatus(moduleId, token);
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManager::ClearILTransformationStatus(_In_ ModuleID moduleId, _In_ mdMethodDef functionToken)
{
    HRESULT hr;
    CComPtr<IModuleInfo> pModuleInfo;
    IfFailRet(m_pAppDomainCollection->GetModuleInfoById(moduleId, &pModuleInfo));
    static_cast<CModuleInfo*>(pModuleInfo.p)->SetMethodIsTransformed(functionToken, false);
    return S_OK;
}
