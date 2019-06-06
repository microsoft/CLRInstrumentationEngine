#pragma once

namespace MicrosoftInstrumentationEngine
{
    struct EventLogItem
    {
        EventLogItem(WORD wEventType, tstring tsEventLog) {
            this->wEventType = wEventType;
            this->tsEventLog = tsEventLog;
        }

        WORD wEventType;
        tstring tsEventLog;
    };

    class CEventLoggingBase
    {
    protected:
        // critical section which protects the shared buffer for reads/writes
        CCriticalSection m_cs;
        std::queue<EventLogItem> m_eventQueue;
        size_t m_eventQueueLength;
        CEvent m_hEventQueueEvent;
        CHandle m_hEventQueueThread;
        HANDLE m_hEventSource; // Not relying on RAII; this handle should be closed by DeregisterEventSource call.
        bool m_isShutdown;

    protected:
        // Registers the EventSource with the given name and sets m_hEventQueueThread handle.
        HRESULT InitializeEventSource(_In_ LPCWSTR wszEventSourceName);

        // Deregisters the m_hEventSource handle.
        HRESULT TerminateEventSource();

        // The COM-initialized thread that m_hEventQueueThread holds; polls the queue for event logs to report.
        static DWORD WINAPI LogEventThreadProc(_In_ LPVOID lpParam);

        virtual void LogEvent(_In_ const tstring& tsEntry);

        // Conducts ReportEvent invocation.
        void LogEvent(_In_ const EventLogItem& tsEntry);

        // Adds the eventlog and type to the queue.
        HRESULT AppendToQueue(_In_ WORD wEventType, _In_ tstring wsEventLog);

    public:
        CEventLoggingBase();
        ~CEventLoggingBase();
    };
}
