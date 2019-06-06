#pragma once

#include "LoggerService.h"
#include "LoggerSink.h"

namespace MicrosoftInstrumentationEngine
{
    class CEventLoggerSink :
        public ILoggerSink
    {
    private:
        // critical section which protects the shared buffer for reads/writes
        CCriticalSection m_cs;
        std::deque<tstring> m_eventQueue;
        size_t m_eventQueueLength;
        CEvent m_hEventQueueEvent;
        CHandle m_hEventQueueThread;
        HANDLE m_hEventSource; // Not relying on RAII; this handle should be closed by DeregisterEventSource call.
        CEvent m_hShutdownEvent;
        CInitOnce m_initEventSource;
        CInitOnce m_shutdown;

    public:
        CEventLoggerSink();
        ~CEventLoggerSink();

        // ILoggerSink Members
    protected:
        HRESULT Initialize(_In_ CLoggerService* pLogging) override;

        void LogMessage(_In_ LPCWSTR wszMessage) override;

        void LogError(_In_ LPCWSTR wszMessage) override;

        void LogDumpMessage(_In_ LPCWSTR wszMessage) override;

        HRESULT Reset(_In_ LoggingFlags defaultFlags, _Out_ LoggingFlags* pEffectiveFlags) override;

        HRESULT Shutdown() override;

    private:
        HRESULT AppendToQueue(_In_ wstring wsEventLog);
        HRESULT InitializeEventSource();
        static bool IsSignaled(_In_ const CEvent& event);
        static DWORD WINAPI LogEventThreadProc(_In_ LPVOID lpParam);
        HRESULT ShutdownCore();

    protected:
        virtual void LogEvent(const tstring& tsEntry);
    };
}