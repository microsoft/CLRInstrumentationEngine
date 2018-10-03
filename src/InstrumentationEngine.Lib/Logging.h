// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"
#include <fstream>
#include <sstream>
#ifndef PLATFORM_UNIX
#include <thread>
#endif
#include <queue>
#include "InstrumentationEngine.h"
#include "File.h"

// CLogging uses *wprintf* methods for formatting strings.
#ifdef PLATFORM_UNIX
// Use %S for WCHAR* on Linux (which is char16_t*) since *wprintf* expects strings to be wchar_t*.
// The capital S specifier means "format the string using the width that is opposite of the formatting method".
// The capital S specifier is appropriate since WCHAR* on Linux is smaller than the *wprintf* width expectation.
#define WCHAR_SPEC _T("%S")
#else
// Use %s for WCHAR* on Windows (which is wchar_t*)
// The lowercase s specifier is appropriate since WCHAR* on Windows matches *wprintf* width expectation.
#define WCHAR_SPEC _T("%s")
#endif

namespace MicrosoftInstrumentationEngine
{
    // Currently, this is a static class which makes it easy to log to. However, if we wind up
    // supporting in-proc sxs, this won't work.
    // This class has grown big enough that it probably shouldn't be static any more
    class CLogging
    {
    public:
        class XmlDumpHelper final
        {
        private:
            tstring m_tag;
            tstring m_indent;
            tstring m_childrenIndent;
            tstringstream m_result;
        public:
            XmlDumpHelper(const WCHAR* tag, const unsigned int indent);
            ~XmlDumpHelper();

            void WriteStringNode(const WCHAR* name, const WCHAR* value);
            void WriteUlongNode(const WCHAR* name, const ULONG32 value);
        };

    private:
#ifdef PLATFORM_UNIX
        static constexpr const char* ERROR_PREFIX_FORMAT = "LogError[%H:%M:%S]:";
        static constexpr const char* MESSAGE_PREFIX_FORMAT = "LogMessage[%H:%M:%S]:";
#else
        static constexpr const WCHAR* ERROR_PREFIX_FORMAT = _T("LogError[%H:%M:%S]:");
        static constexpr const WCHAR* MESSAGE_PREFIX_FORMAT = _T("LogMessage[%H:%M:%S]:");
#endif

    private:
        static volatile LoggingFlags s_loggingFlags;
        static bool s_bEnableDiagnosticLogToDebugPort;
        static volatile LoggingFlags s_enableLoggingToFile;

        static std::unique_ptr<FILE, FILEDeleter> s_outputFile;

        // critical section which protects the static s_pLoggingHost during startup and shutdown and other state
        // such as s_loggingFlags
        // Use GetLoggingHost to make sure this is taken.
        static CCriticalSection s_loggingCs;

        static ATL::CComPtr<IProfilerManagerLoggingHost> s_pLoggingHost;
        // critical section which protects the shared buffer for reads/writes
        static CCriticalSection s_eventLoggingCs;

        static CHandle s_hLogEvent;

        static HANDLE s_hEventSource;

        static std::queue<tstring> s_EventLogMessageQueue;
        static size_t s_queueLength;

    private:
        static void OutputLogFilePrefix(_In_ LoggingFlags loggingFlags);
        static void OutputLogFileLine(_In_ const WCHAR* szLine);
        static void CloseLogFile();

        static void GetLoggingHost(_Out_ IProfilerManagerLoggingHost** ppLoggingHost);

        static LoggingFlags GetCurrentLoggingFlags();

        //LPTHREAD_START_ROUTINE
        static DWORD WINAPI LogReportEvent(_In_ LPVOID lpParam);

        static HRESULT WriteToSharedBuffer(_In_ LPCWSTR wszEventLog);
        static HRESULT ReadFromSharedBuffer(_Out_ tstring& wszEventLog);

    public:
        static void LogMessage(_In_ const WCHAR* szMessage, ...);
        static void LogError(_In_ const WCHAR* szError, ...);
        static void LogDumpMessage(_In_ const WCHAR* szMessage, ...);

        static HRESULT SetLoggingHost(_In_ IProfilerManagerLoggingHost* pLoggingHost);

        static void EnableDiagnosticLogToDebugPort(_In_ bool enable);
        static void EnableLoggingToFile(_In_ LoggingFlags loggingFlags, _In_ const tstring& filePath);

        // Allows instrumentation methods and hosts to ask for the current logging flags
        static HRESULT GetLoggingFlags(_Out_ LoggingFlags* pLoggingFlags);

        // Allows instrumentation methods and hosts to modify the current logging flags
        static HRESULT SetLoggingFlags(_In_ LoggingFlags loggingFlags);

        // Call this to determine if logging should be allowed for a specific log type.
        static bool AllowLogEntry(_In_ LoggingFlags flags);

        // Sets up the EventLogging thread
        static HRESULT InitializeEventLogging();

        static HRESULT TerminateEventLogging();
    };
}