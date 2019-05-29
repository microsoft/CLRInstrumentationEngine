// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "LoggerService.h"
#include "Macros.h"
#include "../InstrumentationEngine.Api/InstrumentationEngine.h"

#ifndef PLATFORM_UNIX
#include "DebugLoggerSink.h"
#include "EventLoggerSink.h"
#endif
#include "FileLoggerSink.h"
#include "HostLoggerSink.h"

using namespace MicrosoftInstrumentationEngine;
using namespace std;

CLoggerService::CLoggerService() :
    m_defaultFlags(LoggingFlags_None),
    m_effectiveFlags(LoggingFlags_None),
    m_fLogToDebugPort(false),
    m_initialize([=]() { return InitializeCore(); })
{
}

CLoggerService::~CLoggerService()
{
    Shutdown();
}

HRESULT CLoggerService::GetLoggingHost(_Out_ IProfilerManagerLoggingHost** ppLoggingHost)
{
    if (!ppLoggingHost)
    {
        return E_POINTER;
    }

    *ppLoggingHost = nullptr;

    if (!m_initialize.IsSuccessful())
    {
        return E_UNEXPECTED;
    }

    CCriticalSectionHolder holder(&m_cs);

    return m_pLoggingHost.CopyTo(ppLoggingHost);
}

HRESULT CLoggerService::SetLoggingHost(_In_ IProfilerManagerLoggingHost* pLoggingHost)
{
    if (!m_initialize.IsSuccessful())
    {
        return E_UNEXPECTED;
    }

    CCriticalSectionHolder holder(&m_cs);

    m_pLoggingHost = pLoggingHost;

    return RecalculateLoggingFlags();
}

HRESULT CLoggerService::GetLoggingFlags(_Out_ LoggingFlags* pLoggingFlags)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pLoggingFlags);

    if (!m_initialize.IsSuccessful())
    {
        return E_UNEXPECTED;
    }

    CCriticalSectionHolder holder(&m_cs);

    *pLoggingFlags = m_defaultFlags;

    return S_OK;
}

HRESULT CLoggerService::SetLoggingFlags(_In_ LoggingFlags loggingFlags)
{
    if (!m_initialize.IsSuccessful())
    {
        return E_UNEXPECTED;
    }

    CCriticalSectionHolder holder(&m_cs);

    m_defaultFlags = loggingFlags;

    return RecalculateLoggingFlags();
}

bool CLoggerService::GetLogToDebugPort()
{
    if (!m_initialize.IsSuccessful())
    {
        return false;
    }

    return m_fLogToDebugPort;
}

void CLoggerService::SetLogToDebugPort(_In_ bool enable)
{
    if (!m_initialize.IsSuccessful())
    {
        return;
    }

    CCriticalSectionHolder holder(&m_cs);

    m_fLogToDebugPort = enable;

    RecalculateLoggingFlags();
}

HRESULT CLoggerService::Initialize()
{
    return m_initialize.Get();
}

HRESULT CLoggerService::InitializeCore()
{
    HRESULT hr = S_OK;

    WCHAR wszLogLevel[MAX_PATH];
    ZeroMemory(wszLogLevel, MAX_PATH);
    if (GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_LogLevel"), wszLogLevel, MAX_PATH) > 0)
    {
        m_defaultFlags = ExtractLoggingFlags(wszLogLevel);
    }

    if (FAILED(CreateSinks(m_allSinks)))
    {
        return hr;
    }

    for (shared_ptr<ILoggerSink>& pSink : m_allSinks)
    {
        if (FAILED(hr = pSink->Initialize(this)))
        {
            return hr;
        }
    }

    return RecalculateLoggingFlags();
}

void CLoggerService::LogMessage(_In_ LPCWSTR wszMessage, va_list argptr)
{
    if (!m_initialize.IsSuccessful())
    {
        return;
    }

    CCriticalSectionHolder holder(&m_cs);

    if (!AllowLogEntry(LoggingFlags_Trace))
    {
        return;
    }

    WCHAR szLogEntry[LogEntryMaxSize];
    _vsnwprintf_s(szLogEntry, LogEntryMaxSize, _TRUNCATE, wszMessage, argptr);

    for (shared_ptr<ILoggerSink>& pSink : m_messageSinks)
    {
        pSink->LogMessage(szLogEntry);
    }
}

void CLoggerService::LogMessage(_In_ LPCWSTR wszMessage, ...)
{    
    va_list argptr;
    va_start (argptr, wszMessage);
    LogMessage(wszMessage, argptr);
    va_end (argptr);
}

void CLoggerService::LogError(_In_ LPCWSTR wszError, va_list argptr)
{
    if (!m_initialize.IsSuccessful())
    {
        return;
    }

    CCriticalSectionHolder holder(&m_cs);

    if (!AllowLogEntry(LoggingFlags_Errors))
    {
        return;
    }

    WCHAR szLogEntry[LogEntryMaxSize];
    _vsnwprintf_s(szLogEntry, LogEntryMaxSize, _TRUNCATE, wszError, argptr);

    for (shared_ptr<ILoggerSink>& pSink : m_errorSinks)
    {
        pSink->LogError(szLogEntry);
    }

    if (IsDebuggerPresent())
    {
        try
        {
            __debugbreak();
        }
        // SEH Exceptions are sent through C++ handlers in the instrumentation engine.
        catch (...)
        {
        }
    }
}

void CLoggerService::LogError(_In_ LPCWSTR wszError, ...)
{
    va_list argptr;
    va_start(argptr, wszError);
    LogError(wszError, argptr);
    va_end(argptr);
}

void CLoggerService::LogDumpMessage(_In_ LPCWSTR wszMessage, va_list argptr)
{
    if (!m_initialize.IsSuccessful())
    {
        return;
    }

    CCriticalSectionHolder holder(&m_cs);

    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }

    WCHAR szLogEntry[LogEntryMaxSize];
    _vsnwprintf_s(szLogEntry, LogEntryMaxSize, _TRUNCATE, wszMessage, argptr);

    for (shared_ptr<ILoggerSink>& pSink : m_dumpSinks)
    {
        pSink->LogDumpMessage(szLogEntry);
    }
}

void CLoggerService::LogDumpMessage(_In_ LPCWSTR wszMessage, ...)
{
    va_list argptr;
    va_start(argptr, wszMessage);
    LogDumpMessage(wszMessage, argptr);
    va_end(argptr);
}

bool CLoggerService::AllowLogEntry(_In_ LoggingFlags flags)
{
    if (!m_initialize.IsSuccessful())
    {
        return false;
    }

    return IsFlagSet(m_effectiveFlags, flags);
}

// static
LoggingFlags CLoggerService::ExtractLoggingFlags(_In_ LPCWSTR wszRequestedFlagNames)
{
    if (nullptr == wszRequestedFlagNames)
    {
        return LoggingFlags_None;
    }

    // If the request is for all logging flags, just return the allowable flags
    if (wcsstr(wszRequestedFlagNames, _T("All")) != nullptr)
    {
        return LoggingFlags_All;
    }

    // For each logging flag, check that the named flag was specified. Combine the results
    // of each (since each result will be a single flag) and return the combination.
    return (LoggingFlags)(
        ExtractLoggingFlag(wszRequestedFlagNames, _T("Errors"), LoggingFlags_Errors) |
        ExtractLoggingFlag(wszRequestedFlagNames, _T("Messages"), LoggingFlags_Trace) |
        ExtractLoggingFlag(wszRequestedFlagNames, _T("Dumps"), LoggingFlags_InstrumentationResults)
        );
}

// static
LoggingFlags CLoggerService::ExtractLoggingFlag(
    _In_ LPCWSTR wszRequestedFlagNames,
    _In_ LPCWSTR wszSingleTestFlagName,
    _In_ LoggingFlags singleTestFlag
)
{
    if (nullptr == wszRequestedFlagNames || nullptr == wszSingleTestFlagName)
    {
        return LoggingFlags_None;
    }

    // Check that the name of the flag is in the string that specifies what types
    // of messages are requested to be logged.
    if (wcsstr(wszRequestedFlagNames, wszSingleTestFlagName) != nullptr)
    {
        return singleTestFlag;
    }

    return LoggingFlags_None;
}

HRESULT CLoggerService::RecalculateLoggingFlags()
{
    HRESULT hr = S_OK;

    m_dumpSinks.clear();
    m_errorSinks.clear();
    m_messageSinks.clear();

    LoggingFlags effectiveFlags = LoggingFlags_None;
    for (shared_ptr<ILoggerSink>& pSink : m_allSinks)
    {
        LoggingFlags sinkFlags;
        if (FAILED(hr = pSink->Reset(m_defaultFlags, &sinkFlags)))
        {
            return hr;
        }

        if (IsFlagSet(sinkFlags, LoggingFlags_Errors))
        {
            m_errorSinks.push_back(pSink);
        }
        if (IsFlagSet(sinkFlags, LoggingFlags_InstrumentationResults))
        {
            m_dumpSinks.push_back(pSink);
        }
        if (IsFlagSet(sinkFlags, LoggingFlags_Trace))
        {
            m_messageSinks.push_back(pSink);
        }

        effectiveFlags = static_cast<LoggingFlags>(effectiveFlags | sinkFlags);
    }

    m_effectiveFlags = effectiveFlags;

    return S_OK;
}

HRESULT CLoggerService::Shutdown()
{
    CCriticalSectionHolder holder(&m_cs);

    HRESULT hr;
    for (shared_ptr<ILoggerSink>& pSink : m_allSinks)
    {
        if (FAILED(hr = pSink->Shutdown()))
        {
            return hr;
        }
    }

    m_allSinks.clear();
    m_dumpSinks.clear();
    m_errorSinks.clear();
    m_messageSinks.clear();

    m_pLoggingHost.Release();

    return S_OK;
}

HRESULT CLoggerService::CreateSinks(std::vector<std::shared_ptr<ILoggerSink>>& sinks)
{
    sinks.push_back(make_shared<CDebugLoggerSink>());
    sinks.push_back(make_shared<CEventLoggerSink>());
    sinks.push_back(make_shared<CFileLoggerSink>());
    sinks.push_back(make_shared<CHostLoggerSink>());

    return S_OK;
}
