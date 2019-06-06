// Copyright (c) Microsoft Corporation. All Rights reserved.
// 

#pragma once

#include "stdafx.h"
#include "../Common.Lib/EventLoggingBase.h"

namespace MicrosoftInstrumentationEngine
{
    class CEventLogger :
        public CEventLoggingBase
    {
    private:
        CInitOnce m_initEventSource;
        static const int LogEntryMaxSize = 4096;

    public:
        CEventLogger();
        ~CEventLogger();

        HRESULT Initialize();

        void LogMessage(_In_ LPCWSTR wszMessage, _In_ va_list argptr);
        void LogWarning(_In_ LPCWSTR wszWarning, _In_ va_list argptr);
        void LogError(_In_ LPCWSTR wszError, _In_ va_list argptr);

        HRESULT Shutdown();

    private:
        void FormatAndAppendToQueue(_In_ WORD wType, _In_ LPCWSTR wszLogEntry, _In_ va_list argptr);
    };
}