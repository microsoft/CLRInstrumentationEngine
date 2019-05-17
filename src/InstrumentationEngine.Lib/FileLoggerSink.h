#pragma once

#include "File.h"
#include "LoggerService.h"

namespace MicrosoftInstrumentationEngine
{
    class CFileLoggerSink :
        public ILoggerSink
    {
    private:
#ifdef PLATFORM_UNIX
        static constexpr const char* ERROR_PREFIX_FORMAT = "LogError[%H:%M:%S]:";
        static constexpr const char* MESSAGE_PREFIX_FORMAT = "LogMessage[%H:%M:%S]:";
#else
        static constexpr const WCHAR* ERROR_PREFIX_FORMAT = _T("LogError[%H:%M:%S]:");
        static constexpr const WCHAR* MESSAGE_PREFIX_FORMAT = _T("LogMessage[%H:%M:%S]:");
#endif

    private:
        LoggingFlags m_fileFlags;
        std::unique_ptr<FILE, FILEDeleter> m_pOutputFile;
        tstring m_tsFilePath;
        tstring m_tsFilePathActual;

    public:
        CFileLoggerSink();
        ~CFileLoggerSink();

        // ILoggerSink Members
    public:
        HRESULT Initialize(_In_ CLoggerService* pLogging) override;

        void LogMessage(_In_ LPCWSTR wszMessage) override;

        void LogError(_In_ LPCWSTR wszMessage) override;

        void LogDumpMessage(_In_ LPCWSTR wszMessage) override;

        HRESULT Reset(_In_ LoggingFlags defaultFlags, _Out_ LoggingFlags* pEffectiveFlags) override;

        HRESULT Shutdown() override;

    private:
        void CloseLogFile();
        HRESULT InitializeLogFile();
        void WritePrefix(_In_ LoggingFlags flags);
        void WriteLine(_In_ LPCWSTR wszMessage);

        // Methods used for testing
    public:
        void SetFlags(_In_ LoggingFlags fileFlags);
        void SetFilePath(_In_ const tstring& tsFilePath);
        const tstring GetEffectiveFilePath();
    };
}