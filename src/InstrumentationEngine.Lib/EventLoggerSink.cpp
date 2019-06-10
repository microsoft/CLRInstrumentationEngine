// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "EventLoggerSink.h"

using namespace MicrosoftInstrumentationEngine;

CEventLoggerSink::CEventLoggerSink() :
    m_initEventSource([=]() { return InitializeEventSource(_T("Instrumentation Engine")); })
{
}

CEventLoggerSink::~CEventLoggerSink()
{
    Shutdown();
}

HRESULT CEventLoggerSink::Initialize(_In_ CLoggerService* pLogging)
{
    return S_OK;
}

void CEventLoggerSink::LogMessage(_In_ LPCWSTR wszMessage)
{
    // Ignore messages
}

void CEventLoggerSink::LogError(_In_ LPCWSTR wszMessage)
{
    AppendToQueue(EVENTLOG_ERROR_TYPE, wszMessage);
}

void CEventLoggerSink::LogDumpMessage(_In_ LPCWSTR wszMessage)
{
    // Ignore dump messages
}

HRESULT CEventLoggerSink::Reset(_In_ LoggingFlags defaultFlags, _Out_ LoggingFlags* pEffectiveFlags)
{
    if (!pEffectiveFlags)
    {
        return E_POINTER;
    }

    *pEffectiveFlags = LoggingFlags_None;

    // Only allow errors to be sent to event log
    LoggingFlags effectiveFlags = static_cast<LoggingFlags>(defaultFlags & LoggingFlags_Errors);

    // Only start event source thread if logging has been enabled
    if (LoggingFlags_None != effectiveFlags)
    {
        HRESULT hr = S_OK;
        IfFailRetNoLog(m_initEventSource.Get());
    }

    *pEffectiveFlags = effectiveFlags;

    return S_OK;
}

HRESULT CEventLoggerSink::Shutdown()
{
    HRESULT hr = TerminateEventSource();
    m_initEventSource.Reset();

    return hr;
}