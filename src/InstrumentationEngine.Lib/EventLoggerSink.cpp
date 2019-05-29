#include "stdafx.h"
#include "EventLoggerSink.h"

using namespace MicrosoftInstrumentationEngine;

CEventLoggerSink::CEventLoggerSink() :
    // We use a small spin count here for when lock contention occurs.
    // Since this lock guards reads and writes to the buffer, contention is short.
    // This spincount allows the thread to spin briefly in hopes that the lock is
    // released quickly and we avoid context switching.
    m_cs(10),
    m_eventQueueLength(0),
    m_hEventQueueEvent(FALSE, FALSE), // auto-reset, initially unsignaled
    m_hEventSource(nullptr),
    m_hShutdownEvent(TRUE, FALSE), // manual-reset, initially unsignaled
    m_initEventSource([=]() { return InitializeEventSource(); }),
    m_shutdown([=]() { return ShutdownCore(); })
{
}

CEventLoggerSink::~CEventLoggerSink()
{
    Shutdown();
}

HRESULT CEventLoggerSink::Initialize(_In_ CLoggerService* pLogging)
{
    return S_OK;
}

void CEventLoggerSink::LogMessage(_In_ LPCWSTR wszMessage)
{
    // Ignore messages
}

void CEventLoggerSink::LogError(_In_ LPCWSTR wszMessage)
{
    AppendToQueue(wszMessage);
}

void CEventLoggerSink::LogDumpMessage(_In_ LPCWSTR wszMessage)
{
    // Ignore dump messages
}

HRESULT CEventLoggerSink::Reset(_In_ LoggingFlags defaultFlags, _Out_ LoggingFlags* pEffectiveFlags)
{
    if (!pEffectiveFlags)
    {
        return E_POINTER;
    }

    *pEffectiveFlags = LoggingFlags_None;

    // Only allow errors to be sent to event log
    LoggingFlags effectiveFlags = static_cast<LoggingFlags>(defaultFlags & LoggingFlags_Errors);

    // Only start event source thread if logging has been enabled
    if (LoggingFlags_None != effectiveFlags)
    {
        HRESULT hr;
        if (FAILED(hr = m_initEventSource.Get()))
        {
            return hr;
        }
    }

    *pEffectiveFlags = effectiveFlags;

    return S_OK;
}

HRESULT CEventLoggerSink::Shutdown()
{
    return m_shutdown.Get();
}

HRESULT CEventLoggerSink::ShutdownCore()
{
    // Signal event source thread to shutdown.
    // Block scope used to release critical section before end of method.
    {
        CCriticalSectionHolder holder(&m_cs);

        // Signal shutdown first
        if (!SetEvent(m_hShutdownEvent))
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        // Signal thread to proceed
        if (!SetEvent(m_hEventQueueEvent))
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

HRESULT CEventLoggerSink::InitializeEventSource()
{
    HRESULT hr = S_OK;

    m_hEventSource = RegisterEventSource(NULL, L"Instrumentation Engine");
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
DWORD WINAPI CEventLoggerSink::LogEventThreadProc(_In_ LPVOID lpParam)
{
    CEventLoggerSink* pThis = static_cast<CEventLoggerSink*>(lpParam);

    // Since ReportEvent requires CoInitialize(NULL) due to their detouring logic of eventlogging messages
    // and since we live inside the user's process (which is not guaranteed to be STA), we must run this logging inside a separate thread.
    // S_OK indicates success
    // S_FALSE indicates already initialized
    HRESULT hr;
    if (FAILED(hr = CoInitialize(nullptr)))
    {
        return hr;
    }

    bool fShutdown = false;

    while (!fShutdown)
    {
        // Wait to be signaled
        DWORD dwWaitResult = WaitForSingleObject(
            pThis->m_hEventQueueEvent,
            INFINITE);

        if (dwWaitResult != WAIT_OBJECT_0)
        {
            return GetLastError();
        }

        // Get copy of queued items and check if shutdown requested
        deque<tstring> eventQueue;

        // Block scope used to release critical section before processing queue items.
        {
            CCriticalSectionHolder holder(&pThis->m_cs);

            // Swap contents of queue
            eventQueue.swap(pThis->m_eventQueue);

            // Reset total queue length; since it was swapped with empty queue
            // the starting length is zero.
            pThis->m_eventQueueLength = 0;

            // Check if shutdown requested
            fShutdown = IsSignaled(pThis->m_hShutdownEvent);
        }

        // Report each item in the swapped queue
        while (!eventQueue.empty())
        {
            tstring& wsLogEntry = eventQueue.front();

            if (wsLogEntry.empty())
            {
                continue; // ignore empty strings
            }

            pThis->LogEvent(wsLogEntry);

            eventQueue.pop_front();
        }
    }

    CoUninitialize();

    return 0;
}

HRESULT CEventLoggerSink::AppendToQueue(_In_ wstring wsEventLog)
{
    CCriticalSectionHolder holder(&m_cs);

    if (!IsSignaled(m_hShutdownEvent))
    {
        // Aribtrarily allow max of 64KB
        if (m_eventQueueLength < 64 * (1 << 10))
        {
            m_eventQueue.push_back(wsEventLog);
            m_eventQueueLength += wsEventLog.length();

            if (!SetEvent(m_hEventQueueEvent))
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

// static
bool CEventLoggerSink::IsSignaled(_In_ const CEvent& event)
{
    return WAIT_OBJECT_0 == WaitForSingleObject(event, 0);
}

void CEventLoggerSink::LogEvent(const tstring& tsEntry)
{
    if (m_hEventSource)
    {
        const WORD wType = EVENTLOG_ERROR_TYPE;
        // Each event source can use event ids 0-65535. An event id is used to associate with an
        // event description registered in the registry by a message file. Since we don't have
        // access to the registry on Antares and the event description wouldn't be very useful
        // (we just spit out the error messages of any instrumentation method), the event id
        // is arbitrarily set.
        const DWORD dwEventId = 1000;

        LPCWSTR wszEntry = tsEntry.c_str();

        // Write to event log. Note: if this fails, we're out of luck
        const BOOL fReport = ReportEvent(
            m_hEventSource, // handle to event log
            wType,          // event type
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