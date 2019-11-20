// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"

#include <fstream>
#include <sstream>
#include <queue>
#include <set>

#ifndef PLATFORM_UNIX
#include <functional>
#include <thread>
#endif

#include "../InstrumentationEngine.Api/InstrumentationEngine.h"
#include "LoggerSink.h"

#include "../Common.Lib/InitOnce.h"

using namespace CommonLib;

namespace MicrosoftInstrumentationEngine
{
    class CLoggerService
    {
    private:
        static const int LogEntryMaxSize = 4096;
        static const LoggingFlags LoggingFlags_All = (LoggingFlags)(LoggingFlags_Errors | LoggingFlags_Trace | LoggingFlags_InstrumentationResults);
        static constexpr const WCHAR* LogLevelEnvironmentVariableName = _T("MicrosoftInstrumentationEngine_LogLevel");

    private:
        // Used to protect all fields
        CCriticalSection m_cs;

        // The flags initially determined by the service or set via the SetLoggingFlags method.
        LoggingFlags m_defaultFlags;
        // The effective set of flags (flags that are in actual use) as determined by querying
        // each of the logger sinks. This is updated each time a logger sink dependency is changed
        // e.g. calling SetLoggingFlags, SetLoggingHost, and SetLogToDebugPort.
        LoggingFlags m_effectiveFlags;
        // This is the cumulative LoggingFlags for all InstrumentationMethods
        LoggingFlags m_instrumentationMethodFlags;

        // This Map contains as keys each of the LoggingFlags and for each key the value is the set of
        // InstrumentationMethods that support the LoggingFlags.
        std::unordered_map<LoggingFlags, std::set<GUID>> m_loggingFlagsToInstrumentationMethodsMap;

        bool m_fLogToDebugPort;
        CInitOnce m_initialize;
        ATL::CComPtr<IProfilerManagerLoggingHost> m_pLoggingHost;

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

        // Updates the logging flags for the specific InstrumentationMethod classId
        HRESULT UpdateInstrumentationMethodFlags(_In_ GUID classId, _In_ LoggingFlags loggingFlags);

        // For the given loggingLevel, updates the Set with adding/removing the classId based on the loggingFlags
        HRESULT UpdateInstrumentationMethodFlagsInternal(_In_ GUID classId, _In_ LoggingFlags loggingFlags, _In_ LoggingFlags loggingLevel);

    public:
        bool AllowLogEntry(_In_ LoggingFlags flags);

        static LoggingFlags ExtractLoggingFlags(_In_ LPCWSTR wszRequestedFlagNames);

        HRESULT Initialize();

        void LogMessage(_In_ LPCWSTR wszMessage, _In_ va_list argptr);
        void LogMessage(_In_ LPCWSTR wszMessage, ...);
        void LogError(_In_ LPCWSTR wszMessage, _In_ va_list argptr);
        void LogError(_In_ LPCWSTR wszError, ...);
        void LogDumpMessage(_In_ LPCWSTR wszMessage, _In_ va_list argptr);
        void LogDumpMessage(_In_ LPCWSTR wszMessage, ...);

        HRESULT GetLoggingHost(_Out_ IProfilerManagerLoggingHost** ppLoggingHost);
        HRESULT SetLoggingHost(_In_ IProfilerManagerLoggingHost* pLoggingHost);

        bool GetLogToDebugPort();
        void SetLogToDebugPort(_In_ bool enable);

        HRESULT GetLoggingFlags(_Out_ LoggingFlags* pLoggingFlags);
        HRESULT SetLoggingFlags(_In_ LoggingFlags loggingFlags);
        HRESULT UpdateInstrumentationMethodLoggingFlags(_In_ GUID classId, _In_ LoggingFlags loggingFlags);

        HRESULT Shutdown();

    protected:
        virtual HRESULT CreateSinks(std::vector<std::shared_ptr<ILoggerSink>>& sinks);
    };
}