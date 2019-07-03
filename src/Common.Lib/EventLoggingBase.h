// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy can be used
#include <atlsync.h>
#include <queue>
#pragma warning(pop)

#include "tstring.h"
#include "CriticalSectionHolder.h"

namespace CommonLib
{
    struct EventLogItem
    {
        EventLogItem(WORD wType, tstring tsLog)
            : wEventType(wType), tsEventLog(tsLog)
        {
        }

        WORD wEventType;
        tstring tsEventLog;
    };

    class CEventLoggingBase
    {
    private:
        // critical section which protects the shared buffer for reads/writes
        CCriticalSection m_cs;
        std::queue<EventLogItem> m_eventQueue;
        size_t m_eventQueueLength;
        CEvent m_hEventQueueFinishedEvent; // Used to signal when thread is done processing items and no longer using event source.
        CEvent m_hEventQueueProcessEvent;
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

        void LogEvent(_In_ const tstring& tsEntry);

        // Conducts ReportEvent invocation.
        virtual void LogEvent(_In_ const EventLogItem& tsEntry);

        // Adds the eventlog and type to the queue.
        HRESULT AppendToQueue(_In_ WORD wEventType, _In_ tstring wsEventLog);

    public:
        CEventLoggingBase();
        ~CEventLoggingBase();
    };
}