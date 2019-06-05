// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "EventLogger.h"

using namespace MicrosoftInstrumentationEngine;

CEventLogger::CEventLogger() :
    m_initEventSource([=]() { return InitializeEventSource(_T("Instrumentation Engine Profiler Proxy")); })
{
}

CEventLogger::~CEventLogger()
{
    Shutdown();
}

HRESULT CEventLogger::Initialize()
{
    return m_initEventSource.Get();
}

void CEventLogger::FormatAndAppendToQueue(_In_ WORD wType, _In_ LPCWSTR wszMessage, _In_ va_list argptr)
{
    IfNotInitRet(m_initEventSource);

    CCriticalSectionHolder holder(&m_cs);

    // Format and truncate the message
    WCHAR wszLogEntry[LogEntryMaxSize];
    _vsnwprintf_s(wszLogEntry, LogEntryMaxSize, _TRUNCATE, wszMessage, argptr);

    AppendToQueue(wType, wszLogEntry);
}

void CEventLogger::LogMessage(_In_ LPCWSTR wszMessage, _In_ va_list argptr)
{
    FormatAndAppendToQueue(EVENTLOG_INFORMATION_TYPE, wszMessage, argptr);
}

void CEventLogger::LogWarning(_In_ LPCWSTR wszWarning, _In_ va_list argptr)
{
    FormatAndAppendToQueue(EVENTLOG_WARNING_TYPE, wszWarning, argptr);
}

void CEventLogger::LogError(_In_ LPCWSTR wszError, _In_ va_list argptr)
{
    FormatAndAppendToQueue(EVENTLOG_ERROR_TYPE, wszError, argptr);

    if (IsDebuggerPresent())
    {
        try
        {
            __debugbreak();
        }
        // SEH Exceptions are sent through C handlers in the instrumentation engine.
        catch (...)
        {
        }
    }
}

HRESULT CEventLogger::Shutdown()
{
    return TerminateEventSource();
}