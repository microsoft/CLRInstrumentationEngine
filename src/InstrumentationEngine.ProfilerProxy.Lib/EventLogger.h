// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"
#include "../Common.Lib/EventLoggingBase.h"
#include "../Common.Lib/InitOnce.h"

using namespace CommonLib;

namespace ProfilerProxy
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

        void LogMessage(_In_ const WCHAR* szMessage, ...);
        void LogWarning(_In_ const WCHAR* szMessage, ...);
        void LogError(_In_ const WCHAR* szError, ...);

        void LogMessage(_In_ LPCWSTR wszMessage, _In_ va_list argptr);
        void LogWarning(_In_ LPCWSTR wszWarning, _In_ va_list argptr);
        void LogError(_In_ LPCWSTR wszError, _In_ va_list argptr);

        HRESULT Shutdown();

    private:
        void FormatAndAppendToQueue(_In_ WORD wType, _In_ LPCWSTR wszLogEntry, _In_ va_list argptr);
    };
}