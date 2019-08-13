// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "DebugLoggerSink.h"

using namespace MicrosoftInstrumentationEngine;

HRESULT CDebugLoggerSink::Initialize(_In_ CLoggerService* pLogging)
{
    m_pLogging = pLogging;
    return S_OK;
}

void CDebugLoggerSink::LogMessage(_In_ LPCWSTR wszMessage)
{
    OutputDebugString(wszMessage);
    OutputDebugString(_T("\r\n"));
}

void CDebugLoggerSink::LogError(_In_ LPCWSTR wszMessage)
{
    OutputDebugString(wszMessage);
    OutputDebugString(_T("\r\n"));
}

void CDebugLoggerSink::LogDumpMessage(_In_ LPCWSTR wszMessage)
{
    OutputDebugString(wszMessage);
}

HRESULT CDebugLoggerSink::Reset(_In_ LoggingFlags defaultFlags, _Out_ LoggingFlags* pEffectiveFlags)
{
    if (!pEffectiveFlags)
    {
        return E_POINTER;
    }

    *pEffectiveFlags = LoggingFlags_None;

    CComPtr<IProfilerManagerLoggingHost> pLoggingHost;
    m_pLogging->GetLoggingHost(&pLoggingHost);

    // Enable all logging via debug port if there is no logging host or it was explicitly asked to log.
    if (!pLoggingHost || m_pLogging->GetLogToDebugPort())
    {
        *pEffectiveFlags = defaultFlags;
    }

    return S_OK;
}