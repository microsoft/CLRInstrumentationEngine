// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "stdafx.h"
#include <fstream>
#include <sstream>
#ifndef PLATFORM_UNIX
#include <functional>
#include <thread>
#endif
#include <queue>
#include "InitOnce.h"
#include "LoggerSink.h"
#include "LoggerSinkProvider.h"
#include "../InstrumentationEngine.Api/InstrumentationEngine.h"

namespace MicrosoftInstrumentationEngine
{
    class CLoggerService
    {
    private:
        static const LoggingFlags LoggingFlags_All = (LoggingFlags)(LoggingFlags_Errors | LoggingFlags_Trace | LoggingFlags_InstrumentationResults);
        static const int MaxLogEntry = 4096;

    private:
        // Used to protect all fields
        CCriticalSection m_cs;

        LoggingFlags m_defaultFlags;
        LoggingFlags m_effectiveFlags;
        bool m_fLogToDebugPort;
        CInitOnce m_initialize;
        ATL::CComPtr<IProfilerManagerLoggingHost> m_pLoggingHost;

        std::shared_ptr<CLoggerSinkProvider> m_pSinksProvider;

        std::vector<std::shared_ptr<ILoggerSink>> m_allSinks;
        std::vector<std::shared_ptr<ILoggerSink>> m_errorSinks;
        std::vector<std::shared_ptr<ILoggerSink>> m_messageSinks;
        std::vector<std::shared_ptr<ILoggerSink>> m_dumpSinks;

    public:
        CLoggerService();
        CLoggerService(const CLoggerService&) = delete;
        ~CLoggerService();

    public:
        CLoggerService& operator=(const CLoggerService&) = delete;

    private:
        static LoggingFlags ExtractLoggingFlag(
            _In_ LPCWSTR wszRequestedFlagNames,
            _In_ LPCWSTR wszSingleTestFlagName,
            _In_ LoggingFlags singleTestFlag
            );

        HRESULT InitializeCore();

        HRESULT RecalculateLoggingFlags();

    public:
        bool AllowLogEntry(_In_ LoggingFlags flags);

        static LoggingFlags ExtractLoggingFlags(_In_ LPCWSTR wszRequestedFlagNames);

        HRESULT Initialize();

        void LogMessage(_In_ LPCWSTR wszMessage, va_list argptr);
        void LogMessage(_In_ LPCWSTR wszMessage, ...);
        void LogError(_In_ LPCWSTR wszMessage, va_list argptr);
        void LogError(_In_ LPCWSTR wszError, ...);
        void LogDumpMessage(_In_ LPCWSTR wszMessage, va_list argptr);
        void LogDumpMessage(_In_ LPCWSTR wszMessage, ...);

        HRESULT GetLoggingHost(_Out_ IProfilerManagerLoggingHost** ppLoggingHost);
        HRESULT SetLoggingHost(_In_ IProfilerManagerLoggingHost* pLoggingHost);

        bool GetLogToDebugPort();
        void SetLogToDebugPort(_In_ bool enable);

        HRESULT GetLoggingFlags(_Out_ LoggingFlags* pLoggingFlags);
        HRESULT SetLoggingFlags(_In_ LoggingFlags loggingFlags);

        HRESULT Shutdown();

        // Methods used for testing
    public:
        void SetProvider(_In_ const std::shared_ptr<CLoggerSinkProvider>& pSinksProvider);
    };
}