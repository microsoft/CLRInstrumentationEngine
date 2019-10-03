// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"
#include "ProfilerManager.h"
#include "ProfilerManagerWrapper.h"

MicrosoftInstrumentationEngine::CProfilerManagerWrapper::CProfilerManagerWrapper(GUID instrumentationMethodGuid, CProfilerManager* pProfilerManager)
{
    WCHAR wszCurrentMethodGuid[40] = { 0 };
    if (0 != ::StringFromGUID2(instrumentationMethodGuid, wszCurrentMethodGuid, 40))
    {
        m_wszInstrumentationMethodGuid = wszCurrentMethodGuid;
    }

    m_pProfilerManager = pProfilerManager;
}

MicrosoftInstrumentationEngine::CProfilerManagerWrapper::~CProfilerManagerWrapper()
{

}

// IProfilerManager Methods
HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::SetupProfilingEnvironment(_In_reads_(numConfigPaths) BSTR rgConfigPaths[], _In_ UINT numConfigPaths)
{
    return m_pProfilerManager->SetupProfilingEnvironment(rgConfigPaths, numConfigPaths);
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::AddRawProfilerHook(_In_ IUnknown *pUnkProfilerCallback)
{
    return m_pProfilerManager->AddRawProfilerHook(pUnkProfilerCallback);
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::RemoveRawProfilerHook()
{
    return m_pProfilerManager->RemoveRawProfilerHook();
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::GetCorProfilerInfo(_Outptr_ IUnknown **ppCorProfiler)
{
    HRESULT hr = S_OK;
    CComPtr<IUnknown> pCorProfiler;
    IfFailRet(m_pProfilerManager->GetCorProfilerInfo(&pCorProfiler));
    *ppCorProfiler = pCorProfiler.Detach();
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::GetRuntimeType(_Out_ COR_PRF_RUNTIME_TYPE* pRuntimeType)
{
    HRESULT hr = S_OK;
    COR_PRF_RUNTIME_TYPE runtimeType;
    IfFailRet(m_pProfilerManager->GetRuntimeType(&runtimeType));
    *pRuntimeType = runtimeType;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::GetProfilerHost(_Out_ IProfilerManagerHost** ppProfilerManagerHost)
{
    HRESULT hr = S_OK;
    CComPtr<IProfilerManagerHost> pProfilerManagerHost;
    IfFailRet(m_pProfilerManager->GetProfilerHost(&pProfilerManagerHost));
    *ppProfilerManagerHost = pProfilerManagerHost.Detach();
    return S_OK;
}

/*
 * ProfilerManagerWrapper will hijack this call and return itself to the Instrumentation Method.
 * This allows ProfilerManagerWrapper to inject the method's ClassID in the logging call.
 */
HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::GetLoggingInstance(_Out_ IProfilerManagerLogging** ppLogging)
{
    HRESULT hr = S_OK;
    *ppLogging = (IProfilerManagerLogging*) this;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::SetLoggingHost(_In_opt_ IProfilerManagerLoggingHost* pLoggingHost)
{
    return m_pProfilerManager->SetLoggingHost(pLoggingHost);
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::GetAppDomainCollection(_Out_ IAppDomainCollection** ppAppDomainCollection)
{
    HRESULT hr = S_OK;
    CComPtr<IAppDomainCollection> pAppDomainCollection;
    IfFailRet(m_pProfilerManager->GetAppDomainCollection(&pAppDomainCollection));
    *ppAppDomainCollection = pAppDomainCollection.Detach();
    return S_OK;
}


HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::CreateSignatureBuilder(_Out_ ISignatureBuilder** ppSignatureBuilder)
{
    HRESULT hr = S_OK;
    CComPtr<ISignatureBuilder> pSignatureBuilder;
    IfFailRet(m_pProfilerManager->CreateSignatureBuilder(&pSignatureBuilder));
    *ppSignatureBuilder = pSignatureBuilder.Detach();
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::GetInstrumentationMethod(_In_ REFGUID cslid, _Out_ IUnknown** ppUnknown)
{
    HRESULT hr = S_OK;
    CComPtr<IUnknown> pUnknown;
    IfFailRet(m_pProfilerManager->GetInstrumentationMethod(cslid, &pUnknown));
    *ppUnknown = pUnknown.Detach();
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::RemoveInstrumentationMethod(_In_ IInstrumentationMethod* pInstrumentationMethod)
{
    return m_pProfilerManager->RemoveInstrumentationMethod(pInstrumentationMethod);
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::AddInstrumentationMethod(
    _In_ BSTR bstrModulePath,
    _In_ BSTR bstrName,
    _In_ BSTR bstrDescription,
    _In_ BSTR bstrModule,
    _In_ BSTR bstrClassGuid,
    _In_ DWORD dwPriority,
    _Out_ IInstrumentationMethod** ppInstrumentationMethod)
{
    HRESULT hr = S_OK;
    CComPtr<IInstrumentationMethod> pInstrumentationMethod;
    IfFailRet(m_pProfilerManager->AddInstrumentationMethod(
        bstrModulePath,
        bstrName,
        bstrDescription,
        bstrModule,
        bstrClassGuid,
        dwPriority,
        &pInstrumentationMethod));
    *ppInstrumentationMethod = pInstrumentationMethod.Detach();
    return S_OK;
}

// IProfilerManager2 Methods
HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::DisableProfiling()
{
    return m_pProfilerManager->DisableProfiling();
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::ApplyMetadata(_In_ IModuleInfo* pMethodInfo)
{
    return m_pProfilerManager->ApplyMetadata(pMethodInfo);
}

// IProfilerManager3 Methods
HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::GetApiVersion(_Out_ DWORD* pApiVer)
{
    HRESULT hr = S_OK;
    DWORD apiVer;
    IfFailRet(m_pProfilerManager->GetApiVersion(&apiVer));
    *pApiVer = apiVer;
    return S_OK;
}

// IProfilerManager4 Methods
HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::GetGlobalLoggingInstance(_Out_ IProfilerManagerLogging** ppLogging)
{
    HRESULT hr = S_OK;
    CComPtr<IProfilerManagerLogging> pLogging;
    IfFailRet(m_pProfilerManager->GetGlobalLoggingInstance(&pLogging));
    *ppLogging = pLogging.Detach();
    return S_OK;
}


// IProfilerManager5 Methods
HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::IsInstrumentationMethodRegistered(_In_ REFGUID clsid, _Out_ BOOL* pfRegistered)
{
    HRESULT hr = S_OK;
    BOOL fRegistered;
    IfFailRet(m_pProfilerManager->IsInstrumentationMethodRegistered(clsid, &fRegistered));
    *pfRegistered = fRegistered;
    return S_OK;
}

// IProfilerManagerLogging Methods
/*
*/
HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::LogMessage(_In_ const WCHAR* wszMessage)
{
    WCHAR wszPrefix[MAX_PATH];
    wcscpy_s(wszPrefix, m_wszInstrumentationMethodGuid.c_str());
    wcscat_s(wszPrefix, wszMessage);
    return m_pProfilerManager->LogMessage(wszPrefix);
}

/*
*/
HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::LogError(_In_ const WCHAR* wszError)
{
    WCHAR wszPrefix[MAX_PATH];
    wcscpy_s(wszPrefix, m_wszInstrumentationMethodGuid.c_str());
    wcscat_s(wszPrefix, wszError);
    return m_pProfilerManager->LogError(wszPrefix);
}

/*
*/
HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::LogDumpMessage(_In_ const WCHAR* wszMessage)
{
    WCHAR wszPrefix[MAX_PATH];
    wcscpy_s(wszPrefix, m_wszInstrumentationMethodGuid.c_str());
    wcscat_s(wszPrefix, wszMessage);
    return m_pProfilerManager->LogDumpMessage(wszPrefix);
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::EnableDiagnosticLogToDebugPort(_In_ BOOL enable)
{
    return m_pProfilerManager->EnableDiagnosticLogToDebugPort(enable);
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::GetLoggingFlags(_Out_ LoggingFlags* pLoggingFlags)
{
    HRESULT hr = S_OK;
    LoggingFlags flags;
    IfFailRet(m_pProfilerManager->GetLoggingFlags(&flags));
    *pLoggingFlags = flags;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CProfilerManagerWrapper::SetLoggingFlags(_In_ LoggingFlags loggingFlags)
{
    return m_pProfilerManager->SetLoggingFlags(loggingFlags);
}