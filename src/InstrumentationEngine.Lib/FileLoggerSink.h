// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "File.h"
#include "LoggerService.h"
#include "LoggerSink.h"

namespace MicrosoftInstrumentationEngine
{
    class CFileLoggerSink :
        public ILoggerSink,
        public IFileLoggerSink
    {
    private:
#ifdef PLATFORM_UNIX
        static constexpr const char* ERROR_PREFIX_FORMAT = "LogError[%H:%M:%S]:";
        static constexpr const char* MESSAGE_PREFIX_FORMAT = "LogMessage[%H:%M:%S]:";
#else
        static constexpr const WCHAR* ERROR_PREFIX_FORMAT = _T("LogError[%H:%M:%S]:");
        static constexpr const WCHAR* MESSAGE_PREFIX_FORMAT = _T("LogMessage[%H:%M:%S]:");
#endif

        static constexpr const WCHAR* LogLevelEnvironmentVariableName = _T("MicrosoftInstrumentationEngine_FileLog");
        static constexpr const WCHAR* LogPathEnvironmentVariableName = _T("MicrosoftInstrumentationEngine_FileLogPath");

    private:
        LoggingFlags m_flags;
        std::unique_ptr<FILE, FILEDeleter> m_pOutputFile;
        tstring m_tsPathActual;
        tstring m_tsPathCandidate;

    public:
        CFileLoggerSink();
        ~CFileLoggerSink();

        // ILoggerSink Members
    protected:
        HRESULT Initialize(_In_ CLoggerService* pLogging) override;

        void LogMessage(_In_ LPCWSTR wszMessage) override;

        void LogError(_In_ LPCWSTR wszMessage) override;

        void LogDumpMessage(_In_ LPCWSTR wszMessage) override;

        HRESULT Reset(_In_ LoggingFlags defaultFlags, _Out_ LoggingFlags* pEffectiveFlags) override;

        HRESULT Shutdown() override;

        // IFileLoggerSink Members
    protected:
        HRESULT SetLogFilePath(_In_ LPCWSTR wszLogFilePath) override;

        HRESULT SetLogFileLevel(_In_ LoggingFlags fileLogFlags) override;

    private:
        void CloseLogFile();
        void WritePrefix(_In_ LoggingFlags flags);
        void WriteLine(_In_ LPCWSTR wszMessage);

    protected:
        const tstring& GetPathActual();
        virtual HRESULT GetPathAndFlags(_Out_ tstring* ptsPath, _Out_ LoggingFlags* pFlags);
    };
}