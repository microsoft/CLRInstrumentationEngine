// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include <string>
#include "ProfilerManager.h"
#include "ProfilerManagerForInstrumentationMethod.h"

#ifndef FORMATTABLE_PREFIX
#define FORMATTABLE_PREFIX _T("[IM:%s]")
#endif

CProfilerManagerForInstrumentationMethod::CProfilerManagerForInstrumentationMethod(GUID instrumentationMethodGuid, CProfilerManager* pProfilerManager)
{
    WCHAR wszCurrentMethodGuid[40] = { 0 };
    if (0 != ::StringFromGUID2(instrumentationMethodGuid, wszCurrentMethodGuid, 40))
    {
        // GUID:36char, Brackets:2char, nullterminator:1char
        // 1 offset = trim start bracket
        // 36 length = keep only Guid portion
        m_wszInstrumentationMethodGuid = tstring(wszCurrentMethodGuid).substr(1, 36);
    }

    m_pProfilerManager = pProfilerManager;
}

// IProfilerManager Methods
HRESULT CProfilerManagerForInstrumentationMethod::SetupProfilingEnvironment(_In_reads_(numConfigPaths) BSTR rgConfigPaths[], _In_ UINT numConfigPaths)
{
    return m_pProfilerManager->SetupProfilingEnvironment(rgConfigPaths, numConfigPaths);
}

HRESULT CProfilerManagerForInstrumentationMethod::AddRawProfilerHook(_In_ IUnknown *pUnkProfilerCallback)
{
    return m_pProfilerManager->AddRawProfilerHook(pUnkProfilerCallback);
}

HRESULT CProfilerManagerForInstrumentationMethod::RemoveRawProfilerHook()
{
    return m_pProfilerManager->RemoveRawProfilerHook();
}

HRESULT CProfilerManagerForInstrumentationMethod::GetCorProfilerInfo(_Outptr_ IUnknown **ppCorProfiler)
{
    return m_pProfilerManager->GetCorProfilerInfo(ppCorProfiler);
}

HRESULT CProfilerManagerForInstrumentationMethod::GetRuntimeType(_Out_ COR_PRF_RUNTIME_TYPE* pRuntimeType)
{
    return m_pProfilerManager->GetRuntimeType(pRuntimeType);
}

HRESULT CProfilerManagerForInstrumentationMethod::GetProfilerHost(_Out_ IProfilerManagerHost** ppProfilerManagerHost)
{
    return m_pProfilerManager->GetProfilerHost(ppProfilerManagerHost);
}

/*
 * ProfilerManagerWrapper will hijack this call and return itself to the Instrumentation Method.
 * This allows ProfilerManagerWrapper to inject the method's ClassID in the logging call.
 */
HRESULT CProfilerManagerForInstrumentationMethod::GetLoggingInstance(_Out_ IProfilerManagerLogging** ppLogging)
{
    HRESULT hr = S_OK;
    CComPtr<IProfilerManagerLogging> pProfilerManagerLogging = (IProfilerManagerLogging*)this;
    *ppLogging = pProfilerManagerLogging.Detach();
    return S_OK;
}

HRESULT CProfilerManagerForInstrumentationMethod::SetLoggingHost(_In_opt_ IProfilerManagerLoggingHost* pLoggingHost)
{
    return m_pProfilerManager->SetLoggingHost(pLoggingHost);
}

HRESULT CProfilerManagerForInstrumentationMethod::GetAppDomainCollection(_Out_ IAppDomainCollection** ppAppDomainCollection)
{
    return m_pProfilerManager->GetAppDomainCollection(ppAppDomainCollection);
}


HRESULT CProfilerManagerForInstrumentationMethod::CreateSignatureBuilder(_Out_ ISignatureBuilder** ppSignatureBuilder)
{
    return m_pProfilerManager->CreateSignatureBuilder(ppSignatureBuilder);
}

HRESULT CProfilerManagerForInstrumentationMethod::GetInstrumentationMethod(_In_ REFGUID cslid, _Out_ IUnknown** ppUnknown)
{
    return m_pProfilerManager->GetInstrumentationMethod(cslid, ppUnknown);
}

HRESULT CProfilerManagerForInstrumentationMethod::RemoveInstrumentationMethod(_In_ IInstrumentationMethod* pInstrumentationMethod)
{
    return m_pProfilerManager->RemoveInstrumentationMethod(pInstrumentationMethod);
}

HRESULT CProfilerManagerForInstrumentationMethod::AddInstrumentationMethod(
    _In_ BSTR bstrModulePath,
    _In_ BSTR bstrName,
    _In_ BSTR bstrDescription,
    _In_ BSTR bstrModule,
    _In_ BSTR bstrClassGuid,
    _In_ DWORD dwPriority,
    _Out_ IInstrumentationMethod** ppInstrumentationMethod)
{
    return m_pProfilerManager->AddInstrumentationMethod(
        bstrModulePath,
        bstrName,
        bstrDescription,
        bstrModule,
        bstrClassGuid,
        dwPriority,
        ppInstrumentationMethod);
}

// IProfilerManager2 Methods
HRESULT CProfilerManagerForInstrumentationMethod::DisableProfiling()
{
    return m_pProfilerManager->DisableProfiling();
}

HRESULT CProfilerManagerForInstrumentationMethod::ApplyMetadata(_In_ IModuleInfo* pMethodInfo)
{
    return m_pProfilerManager->ApplyMetadata(pMethodInfo);
}

// IProfilerManager3 Methods
HRESULT CProfilerManagerForInstrumentationMethod::GetApiVersion(_Out_ DWORD* pApiVer)
{
    return m_pProfilerManager->GetApiVersion(pApiVer);
}

// IProfilerManager4 Methods
HRESULT CProfilerManagerForInstrumentationMethod::GetGlobalLoggingInstance(_Out_ IProfilerManagerLogging** ppLogging)
{
    return m_pProfilerManager->GetGlobalLoggingInstance(ppLogging);
}


// IProfilerManager5 Methods
HRESULT CProfilerManagerForInstrumentationMethod::IsInstrumentationMethodRegistered(_In_ REFGUID clsid, _Out_ BOOL* pfRegistered)
{
    return m_pProfilerManager->IsInstrumentationMethodRegistered(clsid, pfRegistered);
}

// IProfilerManagerLogging Methods
/*
 * Inject Instrumentation Method ClassId
 */
HRESULT CProfilerManagerForInstrumentationMethod::LogMessage(_In_ const WCHAR* wszMessage)
{
    HRESULT hr = S_OK;
    WCHAR wszBuffer[LogEntryMaxSize] = { 0 };
    tstring tsEscapedMessage;
    EscapeFormatSpecifiers(wszMessage, tsEscapedMessage);
    IfFailRetErrno(wcscat_s(wszBuffer, LogEntryMaxSize, FORMATTABLE_PREFIX));
    IfFailRetErrno(wcscat_s(wszBuffer, LogEntryMaxSize, tsEscapedMessage.c_str()));

    return m_pProfilerManager->LogMessageEx(wszBuffer, m_wszInstrumentationMethodGuid.c_str());
}

/*
 * Inject Instrumentation Method ClassId
 */
HRESULT CProfilerManagerForInstrumentationMethod::LogError(_In_ const WCHAR* wszError)
{
    HRESULT hr = S_OK;
    WCHAR wszBuffer[LogEntryMaxSize] = { 0 };
    tstring tsEscapedMessage;
    EscapeFormatSpecifiers(wszError, tsEscapedMessage);
    IfFailRetErrno(wcscat_s(wszBuffer, LogEntryMaxSize, FORMATTABLE_PREFIX));
    IfFailRetErrno(wcscat_s(wszBuffer, LogEntryMaxSize, tsEscapedMessage.c_str()));

    return m_pProfilerManager->LogErrorEx(wszBuffer, m_wszInstrumentationMethodGuid.c_str());
}

/*
 * Inject Instrumentation Method ClassId
 */
HRESULT CProfilerManagerForInstrumentationMethod::LogDumpMessage(_In_ const WCHAR* wszMessage)
{
    HRESULT hr = S_OK;
    WCHAR wszBuffer[LogEntryMaxSize] = { 0 };
    tstring tsEscapedMessage;
    EscapeFormatSpecifiers(wszMessage, tsEscapedMessage);
    IfFailRetErrno(wcscat_s(wszBuffer, LogEntryMaxSize, FORMATTABLE_PREFIX));
    IfFailRetErrno(wcscat_s(wszBuffer, LogEntryMaxSize, tsEscapedMessage.c_str()));
    return m_pProfilerManager->LogDumpMessageEx(wszBuffer, m_wszInstrumentationMethodGuid.c_str());
}

HRESULT CProfilerManagerForInstrumentationMethod::EnableDiagnosticLogToDebugPort(_In_ BOOL enable)
{
    return m_pProfilerManager->EnableDiagnosticLogToDebugPort(enable);
}

HRESULT CProfilerManagerForInstrumentationMethod::GetLoggingFlags(_Out_ LoggingFlags* pLoggingFlags)
{
    return m_pProfilerManager->GetLoggingFlags(pLoggingFlags);
}

HRESULT CProfilerManagerForInstrumentationMethod::SetLoggingFlags(_In_ LoggingFlags loggingFlags)
{
    return m_pProfilerManager->SetLoggingFlags(loggingFlags);
}

void MicrosoftInstrumentationEngine::EscapeFormatSpecifiers(_In_ const tstring tsOriginal, _Inout_ tstring& tsEscaped)
{
    for (tstring::const_iterator it = tsOriginal.begin(); it < tsOriginal.end(); it++)
    {
        // This duplicates any single '%' character (ie. "%%") in the
        // format string from being treated as a format specifier.
        if (*it == L'%')
        {
            tsEscaped.push_back(*it);
        }

        tsEscaped.push_back(*it);
    }
}

#undef FORMATTABLE_PREFIX