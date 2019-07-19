// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "EventLoggingBase.h"

using namespace CommonLib;

CEventLoggingBase::CEventLoggingBase() :
    // We use a small spin count here for when lock contention occurs.
    // Since this lock guards reads and writes to the buffer, contention is short.
    // This spincount allows the thread to spin briefly in hopes that the lock is
    // released quickly and we avoid context switching.
    m_cs(10),
    m_eventQueueLength(0),
    // Event used to tell the processing thread to process the current items
    // on the queue.
    m_hEventQueueProcessEvent(FALSE, FALSE), // auto-reset, initially unsignaled
    m_hEventSource(nullptr),
    m_isShutdown(false)
{
}

CEventLoggingBase::~CEventLoggingBase()
{
}

HRESULT CEventLoggingBase::InitializeEventSource(_In_ LPCWSTR wszEventSourceName)
{
    HRESULT hr = S_OK;

    m_hEventSource = RegisterEventSource(NULL, wszEventSourceName);
    if (!m_hEventSource)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HANDLE hEventQueueThread = CreateThread(
        nullptr,            // default security
        0,                  // default stack size
        LogEventThreadProc, // name of the thread function
        this,               // instance of CEventLogSink
        0,                  // default startup flags
        nullptr             // thread id
    );

    if (nullptr == hEventQueueThread)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    m_hEventQueueThread.Attach(hEventQueueThread);

    return S_OK;
}

//static
DWORD WINAPI CEventLoggingBase::LogEventThreadProc(_In_ LPVOID lpParam)
{
    CEventLoggingBase* pThis = static_cast<CEventLoggingBase*>(lpParam);

    // Since ReportEvent requires CoInitialize(NULL) due to their detouring logic of eventlogging messages
    // and since we live inside the user's process (which is not guaranteed to be STA), we must run this logging inside a separate thread.
    // S_OK indicates success
    // S_FALSE indicates already initialized
    HRESULT hr;
    if (FAILED(hr = CoInitialize(nullptr)))
    {
        return HRESULT_CODE(hr);
    }

    bool fShutdown = false;

    while (!fShutdown)
    {
        // Wait to be signaled
        DWORD dwWaitResult = WaitForSingleObject(
            pThis->m_hEventQueueProcessEvent,
            INFINITE);

        if (dwWaitResult != WAIT_OBJECT_0)
        {
            return GetLastError();
        }

        // Get copy of queued items and check if shutdown requested
        std::queue<EventLogItem> eventQueue;

        // Block scope used to release critical section before processing queue items.
        {
            CCriticalSectionHolder holder(&pThis->m_cs);

            // Swap contents of queue
            eventQueue.swap(pThis->m_eventQueue);

            // Reset total queue length; since it was swapped with empty queue
            // the starting length is zero.
            pThis->m_eventQueueLength = 0;

            // Check if shutdown requested
            fShutdown = pThis->m_isShutdown;
        }

        // Report each item in the swapped queue
        while (!eventQueue.empty())
        {
            EventLogItem& logItem = eventQueue.front();

            // Ignore empty items
            if (!logItem.tsEventLog.empty())
            {
                pThis->LogEvent(logItem);
            }

            eventQueue.pop();
        }
    }

    CoUninitialize();

    return 0;
}

HRESULT CEventLoggingBase::TerminateEventSource()
{
    // Signal event source thread to shutdown.
    // Block scope used to release critical section before end of method.
    {
        CCriticalSectionHolder holder(&m_cs);

        // Signal shutdown first
        m_isShutdown = true;

        // Signal thread to process items
        if (!SetEvent(m_hEventQueueProcessEvent))
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }
    }

    // Allow thread to drain the queue
    WaitForSingleObject(m_hEventQueueThread, INFINITE);

    // Block scope used to release critical section before end of method.
    {
        CCriticalSectionHolder holder(&m_cs);

        if (m_hEventSource)
        {
            if (!DeregisterEventSource(m_hEventSource))
            {
                return HRESULT_FROM_WIN32(GetLastError());
            }
        }
    }

    return S_OK;
}

void CEventLoggingBase::LogEvent(_In_ const tstring& tsEntry)
{
    EventLogItem eventLogItem(EVENTLOG_ERROR_TYPE, tsEntry);
    LogEvent(eventLogItem);
}

void CEventLoggingBase::LogEvent(_In_ const EventLogItem& eventLogItem)
{
    if (m_hEventSource)
    {
        // Each event source can use event ids 0-65535. An event id is used to associate with an
        // event description registered in the registry by a message file. Since we don't have
        // access to the registry on Antares and the event description wouldn't be very useful
        // (we just spit out the error messages of any instrumentation method), the event id
        // is arbitrarily set.
        const DWORD dwEventId = 1000;
        LPCWSTR wszEntry = eventLogItem.tsEventLog.c_str();

        // Write to event log. Note: if this fails, we're out of luck
        const BOOL fReport = ReportEvent(
            m_hEventSource, // handle to event log
            eventLogItem.wEventType,          // event type
            0,              // event category
            dwEventId,      // event id
            nullptr,        // SID of the current user
            1,              // number of strings in szStringData
            0,              // size of binary data in hrErr
            &wszEntry,      // string data in the event
            nullptr         // binary data in the event
        );
    }
}

HRESULT CEventLoggingBase::AppendToQueue(_In_ WORD wEventType, _In_ tstring tsEventLog)
{
    CCriticalSectionHolder holder(&m_cs);

    if (!m_isShutdown)
    {
        // Aribtrarily allow max of 64KB
        if (m_eventQueueLength < 64 * (1 << 10) && !tsEventLog.empty())
        {
            EventLogItem logItem(wEventType, tsEventLog);
            m_eventQueue.push(logItem);
            m_eventQueueLength += tsEventLog.length();

            if (!SetEvent(m_hEventQueueProcessEvent))
            {
                return HRESULT_FROM_WIN32(GetLastError());
            }
        }
        else
        {
            return E_NOT_SUFFICIENT_BUFFER;
        }
    }

    return S_OK;
}
