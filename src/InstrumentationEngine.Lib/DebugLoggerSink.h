#pragma once

#include "LoggerService.h"

namespace MicrosoftInstrumentationEngine
{
    class CDebugLoggerSink :
        public ILoggerSink
    {
    private:
        CLoggerService* m_pLogging;

        // ILoggerSink Members
    public:
        HRESULT Initialize(_In_ CLoggerService* pLogging) override;

        void LogMessage(_In_ LPCWSTR wszMessage) override;

        void LogError(_In_ LPCWSTR wszMessage) override;

        void LogDumpMessage(_In_ LPCWSTR wszMessage) override;

        HRESULT Reset(_In_ LoggingFlags defaultFlags, _Out_ LoggingFlags* pEffectiveFlags) override;
    };
}