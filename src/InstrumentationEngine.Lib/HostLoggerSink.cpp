// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "HostLoggerSink.h"

using namespace MicrosoftInstrumentationEngine;

HRESULT CHostLoggerSink::Initialize(_In_ CLoggerService* pLogging)
{
    m_pLogging = pLogging;
    return S_OK;
}

void CHostLoggerSink::LogMessage(_In_ LPCWSTR wszMessage)
{
    m_pLoggingHost->LogMessage(wszMessage);
    m_pLoggingHost->LogMessage(_T("\r\n"));
}

void CHostLoggerSink::LogError(_In_ LPCWSTR wszMessage)
{
    m_pLoggingHost->LogError(wszMessage);
    m_pLoggingHost->LogError(_T("\r\n"));
}

void CHostLoggerSink::LogDumpMessage(_In_ LPCWSTR wszMessage)
{
    m_pLoggingHost->LogDumpMessage(wszMessage);
}

HRESULT CHostLoggerSink::Reset(_In_ LoggingFlags defaultFlags, _Out_ LoggingFlags* pEffectiveFlags)
{
    if (!pEffectiveFlags)
    {
        return E_POINTER;
    }

    *pEffectiveFlags = LoggingFlags_None;

    m_pLogging->GetLoggingHost(&m_pLoggingHost.p);

    // Enable host logging if there is a logging host
    if (m_pLoggingHost)
    {
        *pEffectiveFlags = defaultFlags;
    }

    return S_OK;
}