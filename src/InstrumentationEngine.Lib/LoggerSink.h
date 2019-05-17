#pragma once

namespace MicrosoftInstrumentationEngine
{
    class CLoggerService;

    class ILoggerSink
    {
    public:
        virtual HRESULT Initialize(_In_ CLoggerService* pLogging) = 0;

        virtual void LogMessage(_In_ LPCWSTR wszMessage) = 0;

        virtual void LogError(_In_ LPCWSTR wszMessage) = 0;

        virtual void LogDumpMessage(_In_ LPCWSTR wszMessage) = 0;

        virtual HRESULT Reset(_In_ LoggingFlags defaultFlags, _Out_ LoggingFlags* pEffectiveFlags) = 0;

        virtual HRESULT Shutdown() { return S_OK; };
    };
}