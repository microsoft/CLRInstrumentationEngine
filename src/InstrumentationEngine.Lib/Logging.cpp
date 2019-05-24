// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "Logging.h"
#include "Macros.h"
#include "InstrumentationEngine.h"

volatile LoggingFlags MicrosoftInstrumentationEngine::CLogging::s_loggingFlags = LoggingFlags_None;
volatile LoggingFlags MicrosoftInstrumentationEngine::CLogging::s_enableLoggingToFile = LoggingFlags_None;
bool MicrosoftInstrumentationEngine::CLogging::s_bEnableDiagnosticLogToDebugPort = false;

CCriticalSection MicrosoftInstrumentationEngine::CLogging::s_loggingCs;

#ifndef PLATFORM_UNIX
// We use a small spin count here for when lock contension occurs.
// Since this lock guards reads and writes to the buffer, contension is short.
// This spincount allows the thread to spin briefly in hopes that the lock is
// released quickly and we avoid context switching.
CCriticalSection MicrosoftInstrumentationEngine::CLogging::s_eventLoggingCs(10);
CHandle MicrosoftInstrumentationEngine::CLogging::s_hLogEvent;
HANDLE MicrosoftInstrumentationEngine::CLogging::s_hEventSource; // Not relying on RAII; this handle should be closed by DeregisterEventSource call.
std::queue<tstring> MicrosoftInstrumentationEngine::CLogging::s_EventLogMessageQueue;
size_t MicrosoftInstrumentationEngine::CLogging::s_queueLength;
#endif

CComPtr<IProfilerManagerLoggingHost> MicrosoftInstrumentationEngine::CLogging::s_pLoggingHost;

std::unique_ptr<FILE, FILEDeleter> MicrosoftInstrumentationEngine::CLogging::s_outputFile;

CInitOnce MicrosoftInstrumentationEngine::CLogging::s_initialize([=]() { return InitializeCore(); });
volatile unsigned int MicrosoftInstrumentationEngine::CLogging::s_usageCount = 0;

// static
HRESULT MicrosoftInstrumentationEngine::CLogging::SetLoggingHost(_In_ IProfilerManagerLoggingHost* pLoggingHost)
{
    if (!s_initialize.IsSuccessful())
    {
        return E_UNEXPECTED;
    }

    CCriticalSectionHolder holder(&s_loggingCs);
    s_pLoggingHost = pLoggingHost;
    return S_OK;
}

// Allows instrumentation methods and hosts to ask for the current logging level
// static
HRESULT MicrosoftInstrumentationEngine::CLogging::GetLoggingFlags(_Out_ LoggingFlags* pLoggingFlags)
{
    if (!s_initialize.IsSuccessful())
    {
        return E_UNEXPECTED;
    }

    HRESULT hr = S_OK;

    IfNullRetPointer(pLoggingFlags);

    *pLoggingFlags = GetCurrentLoggingFlags();

    return S_OK;
}

// Allows instrumentation methods and hosts to modify the current logging level
// static
HRESULT MicrosoftInstrumentationEngine::CLogging::SetLoggingFlags(_In_ LoggingFlags loggingFlags)
{
    if (!s_initialize.IsSuccessful())
    {
        return E_UNEXPECTED;
    }

    return SetLoggingFlagsCore(loggingFlags);
}

HRESULT MicrosoftInstrumentationEngine::CLogging::SetLoggingFlagsCore(_In_ LoggingFlags loggingFlags)
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder holder(&s_loggingCs);

    s_loggingFlags = loggingFlags;

    return S_OK;
}

// static
LoggingFlags MicrosoftInstrumentationEngine::CLogging::GetCurrentLoggingFlags()
{
    CCriticalSectionHolder holder(&s_loggingCs);

    return s_loggingFlags;
}

//static
void MicrosoftInstrumentationEngine::CLogging::EnableDiagnosticLogToDebugPort(_In_ bool enable)
{
    if (!s_initialize.IsSuccessful())
    {
        return;
    }

    s_bEnableDiagnosticLogToDebugPort = enable;
}

//static
void MicrosoftInstrumentationEngine::CLogging::EnableLoggingToFile(_In_ LoggingFlags loggingFlags, _In_ const tstring& filePath)
{
    if (!s_initialize.IsSuccessful())
    {
        return;
    }

    EnableLoggingToFileCore(loggingFlags, filePath);
}

// static
void MicrosoftInstrumentationEngine::CLogging::EnableLoggingToFileCore(_In_ LoggingFlags loggingFlags, _In_ const tstring& filePath)
{
    CloseLogFile();

    if (loggingFlags != LoggingFlags_None)
    {
        tstring tsCandidateFilePath(filePath);
        if (tsCandidateFilePath.empty())
        {
            WCHAR buffer[MAX_PATH + 1];
            GetTempPath(MAX_PATH, buffer);
            tsCandidateFilePath = buffer;
        }

        // Split the provided path in parts
        WCHAR wszDrive[_MAX_DRIVE];
        WCHAR wszDirectory[_MAX_DIR];
        WCHAR wszFile[_MAX_FNAME];
        WCHAR wszExtension[_MAX_EXT];
        _wsplitpath_s(
            tsCandidateFilePath.c_str(),
            wszDrive,
            _MAX_DRIVE,
            wszDirectory,
            _MAX_DIR,
            wszFile,
            _MAX_FNAME,
            wszExtension,
            _MAX_EXT);

        tstring tsFile(wszFile);
        tstring tsExtension(wszExtension);

        // The file name is empty if (a) the last character in the path is a path separator
        // or (b) if no file name is given e.g. the file starts with a period.
        // Generate a file name if the file name is empty and the ext is either empty or "."
        if (tsFile.empty() && (0 == tsExtension.compare(_T(".")) || 0 == tsExtension.compare(_T(""))))
        {
            tsFile.assign(_T("ProfilerLog_"));

            WCHAR szPid[MAX_PATH];
            swprintf_s(szPid, MAX_PATH, _T("%u"), GetCurrentProcessId());
            tsFile.append(szPid);

            tsExtension.clear();
            tsExtension.assign(_T(".txt"));
        }

        // Combine path parts together for overall path
        WCHAR wszFilePath[MAX_PATH];
        _wmakepath_s(
            wszFilePath,
            MAX_PATH,
            wszDrive,
            wszDirectory,
            tsFile.c_str(),
            tsExtension.c_str());

        // _wfsopen just returns nullptr on PLATFORM_UNIX
#ifdef PLATFORM_UNIX
        s_outputFile.reset(_wfopen(wszFilePath, _T("a")));
#else
        s_outputFile.reset(_wfsopen(wszFilePath, _T("a"), _SH_DENYWR));
#endif
        if (!s_outputFile)
        {
            CLogging::LogError(_T("Unable to create or open log file at '%s'."), wszFilePath);
        }
    }
    else
    {
        CloseLogFile();
    }

    s_enableLoggingToFile = loggingFlags;
}

void MicrosoftInstrumentationEngine::CLogging::LogMessage(_In_ const WCHAR* szLine, ...)
{
    if (!s_initialize.IsSuccessful())
    {
        return;
    }

    CCriticalSectionHolder holder(&s_loggingCs);

    if (!AllowLogEntry(LoggingFlags_Trace) && !s_bEnableDiagnosticLogToDebugPort)
    {
        // Verbose logging is not enabled.
        return;
    }

    va_list argptr;
    va_start (argptr, szLine);

    const int maxLogEntry = 4096;
    WCHAR szLogEntry[maxLogEntry];
    _vsnwprintf_s(szLogEntry, maxLogEntry, _TRUNCATE, szLine, argptr);
    va_end (argptr);

    CComPtr<IProfilerManagerLoggingHost> pLoggingHost;
    CLogging::GetLoggingHost(&pLoggingHost);

    if (pLoggingHost == NULL || s_bEnableDiagnosticLogToDebugPort)
    {
#ifndef PLATFORM_UNIX
        OutputDebugString(szLogEntry);
        OutputDebugString(L"\r\n");
#endif
    }

    if (IsFlagSet(s_enableLoggingToFile, LoggingFlags_Trace))
    {
        OutputLogFilePrefix(LoggingFlags_Trace);
        OutputLogFileLine(szLogEntry);
    }

    if ((pLoggingHost != NULL) && (IsFlagSet(s_loggingFlags, LoggingFlags_Trace)))
    {
        pLoggingHost->LogMessage(szLogEntry);
        pLoggingHost->LogMessage(_T("\r\n"));
    }
}

// static
void MicrosoftInstrumentationEngine::CLogging::GetLoggingHost(_Out_ IProfilerManagerLoggingHost** ppLoggingHost)
{
    if (!s_initialize.IsSuccessful())
    {
        return;
    }

    CCriticalSectionHolder holder(&s_loggingCs);

    HRESULT result = s_pLoggingHost.CopyTo(ppLoggingHost);
}

// static
HRESULT MicrosoftInstrumentationEngine::CLogging::InitializeEventLogging()
{
#ifndef PLATFORM_UNIX
    // Only register once! Each snapshotholder instance runs this initialization codepath.
    // Any subsequent initialization after the first will cause the previous handle to close incorrectly.
    if (s_hEventSource)
    {
        return S_FALSE;
    }

    s_queueLength = 0;
    s_hEventSource = RegisterEventSource(NULL, L"Instrumentation Engine");
    if (NULL == s_hEventSource)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HANDLE eventHandle = CreateEvent(
        NULL,       // default security attributes
        FALSE,      // manual-reset event
        FALSE,      // initial state is nonsignaled
        NULL        // object name (NULL means the event is unnamed and local to this process).
    );

    if (eventHandle != nullptr)
    {
        s_hLogEvent.Attach(eventHandle);
    }
    else
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    if (NULL == CreateThread(
        NULL,              // default security
        0,                 // default stack size
        LogReportEvent,    // name of the thread function
        NULL,              // no thread parameters
        0,                 // default startup flags
        NULL               // thread id
    ))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
#endif

    return S_OK;
}

// static
HRESULT MicrosoftInstrumentationEngine::CLogging::TerminateEventLogging()
{
#ifndef PLATFORM_UNIX
    if (s_hEventSource)
    {
        if (FALSE == DeregisterEventSource(s_hEventSource))
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }
    }
#endif
    return S_OK;
}

// static
void MicrosoftInstrumentationEngine::CLogging::LogError(_In_ const WCHAR* szLine, ...)
{
    if (!s_initialize.IsSuccessful())
    {
        return;
    }

    CCriticalSectionHolder holder(&s_loggingCs);

    if (!AllowLogEntry(LoggingFlags_Errors))
    {
        return;
    }

    va_list argptr;
    va_start (argptr, szLine);

    const int maxLogEntry = 4096;
    WCHAR szLogEntry[maxLogEntry];
    _vsnwprintf_s(szLogEntry, maxLogEntry, _TRUNCATE, szLine, argptr);
    va_end (argptr);

    // EventLog
    WriteToSharedBuffer((LPCWSTR)szLogEntry);

    CComPtr<IProfilerManagerLoggingHost> pLoggingHost;
    CLogging::GetLoggingHost(&pLoggingHost);

    if (pLoggingHost == NULL || s_bEnableDiagnosticLogToDebugPort)
    {
#ifndef PLATFORM_UNIX
        OutputDebugString(szLogEntry);
        OutputDebugString(_T("\r\n"));
#endif
    }

    if ((pLoggingHost != NULL) && (IsFlagSet(s_loggingFlags, LoggingFlags_Errors)))
    {
        pLoggingHost->LogError(szLogEntry);
    }

    if (IsFlagSet(s_enableLoggingToFile, LoggingFlags_Errors))
    {
        OutputLogFilePrefix(LoggingFlags_Errors);
        OutputLogFileLine(szLogEntry);
    }

    if (IsDebuggerPresent())
    {
        try
        {
            __debugbreak();
        }
        // SEH Exceptions are sent through C++ handlers in the instrumentation engine.
        catch (...)
        {
        }
    }
}

//static
DWORD WINAPI MicrosoftInstrumentationEngine::CLogging::LogReportEvent(_In_ LPVOID lpParam)
{
#ifndef PLATFORM_UNIX
    // lpParam not used.
    UNREFERENCED_PARAMETER(lpParam);

    // Since ReportEvent requires CoInitialize(NULL) due to their detouring logic of eventlogging messages
    // and since we live inside the user's process (which is not guaranteed to be STA), we must run this logging inside a separate thread.
    // S_OK indicates success
    // S_FALSE indicates already initialized
    HRESULT hr = CoInitialize(NULL);
    IfFailRet(hr);

    while (true)
    {
        // blocked wait for event
        DWORD dwWaitResult = WaitForSingleObject(
            s_hLogEvent, // global event handle
            INFINITE     // wait forever
        );

        // Event Object signal
        if (dwWaitResult != WAIT_OBJECT_0)
        {
            return GetLastError();
        }

        while (!s_EventLogMessageQueue.empty())
        {
            tstring wsLogEntry;
            ReadFromSharedBuffer(wsLogEntry);
            if (wsLogEntry.empty())
            {
                continue; // ignore empty strings
            }

            LPCWSTR wszLogEntry = wsLogEntry.c_str();
            if (s_hEventSource)
            {
                const WORD wType = EVENTLOG_ERROR_TYPE;
                // Each event source can use event ids 0-65535. An event id is used to associate with an
                // event description registered in the registry by a message file. Since we don't have
                // access to the registry on Antares and the event description wouldn't very useful
                // (we just spit out the error messages of any instrumentation method), the event id
                // is arbitrarily set.
                const DWORD dwEventId = 1000;

                // Write to event log. Note: if this fails, we're out of luck
                const BOOL fReport = ReportEvent(
                    s_hEventSource,         // handle to event log
                    wType,                  // event type
                    0,                      // event category
                    dwEventId,              // event id
                    NULL,                   // SID of the current user
                    1,                      // number of strings in szStringData
                    0,                      // size of binary data in hrErr
                    &wszLogEntry,           // string data in the event
                    NULL                    // binary data in the event
                );
            }
        }
    }
#endif

    return 0;
}

//static
HRESULT MicrosoftInstrumentationEngine::CLogging::WriteToSharedBuffer(_In_ LPCWSTR wszEventLog)
{
#ifndef PLATFORM_UNIX
    CCriticalSectionHolder holder(&s_eventLoggingCs);

    // Aribtrarily allow max of 64KB
    if (s_queueLength < 64*(1<<10))
    {
        s_EventLogMessageQueue.push(tstring(wszEventLog));
        s_queueLength += s_EventLogMessageQueue.front().length();
        // Signal reader event:
        SetEvent(s_hLogEvent);
    }
    else
    {
        return E_NOT_SUFFICIENT_BUFFER;
    }
#endif

    return S_OK;
}

//static
HRESULT MicrosoftInstrumentationEngine::CLogging::ReadFromSharedBuffer(_Out_ tstring& wszEventLog)
{
    wszEventLog = _T("");

#ifndef PLATFORM_UNIX
    CCriticalSectionHolder holder(&s_eventLoggingCs);
    if (s_EventLogMessageQueue.empty())
    {
        return ERROR_EMPTY;
    }
    else
    {
        // move() causes us to enter a weird state where the front of the queue will contain garbage temporarily.
        // This is ok since the front is immediately popped off from the queue. We avoid assigning front() to a local variable
        // so we don't use the copy assignment operator.
        wszEventLog = std::move(s_EventLogMessageQueue.front());
        s_queueLength -= wszEventLog.length();
        s_EventLogMessageQueue.pop();
    }
#endif

    return S_OK;
}

//static
void MicrosoftInstrumentationEngine::CLogging::LogDumpMessage(_In_ const WCHAR* szLine, ...)
{
    if (!s_initialize.IsSuccessful())
    {
        return;
    }

    CCriticalSectionHolder holder(&s_loggingCs);

    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }

    va_list argptr;
    va_start (argptr, szLine);

    const int maxLogEntry = 4096;
    WCHAR szLogEntry[maxLogEntry];
    _vsnwprintf_s(szLogEntry, maxLogEntry, _TRUNCATE, szLine, argptr);
    va_end (argptr);

    CComPtr<IProfilerManagerLoggingHost> pLoggingHost;
    CLogging::GetLoggingHost(&pLoggingHost);

    if (pLoggingHost == NULL || s_bEnableDiagnosticLogToDebugPort)
    {
#ifndef PLATFORM_UNIX
        OutputDebugString(szLogEntry);
        OutputDebugString(_T("\r\n"));
#endif
    }

    if ((pLoggingHost != NULL) && (IsFlagSet(s_loggingFlags, LoggingFlags_InstrumentationResults)))
    {
        pLoggingHost->LogDumpMessage(szLogEntry);
    }

    if (IsFlagSet(s_enableLoggingToFile, LoggingFlags_InstrumentationResults))
    {
        OutputLogFileLine(szLogEntry);
    }
}

void MicrosoftInstrumentationEngine::CLogging::OutputLogFilePrefix(_In_ LoggingFlags loggingFlags)
{
    // Use UTF-16 on Windows to avoid string encoding conversions from multi-byte to Unicode.
#ifdef PLATFORM_UNIX
    const char* szFormat = nullptr;
#else
    const WCHAR* szFormat = nullptr;
#endif

    if (loggingFlags == LoggingFlags_Errors)
    {
        szFormat = ERROR_PREFIX_FORMAT;
    }
    else if (loggingFlags == LoggingFlags_Trace)
    {
        szFormat = MESSAGE_PREFIX_FORMAT;
    }

    if (nullptr != szFormat)
    {
        std::time_t currentTime = std::time(nullptr);
        std::tm localTime = { 0 };
#ifdef PLATFORM_UNIX
        if (nullptr != localtime_r(&currentTime, &localTime))
#else
        if (localtime_s(&localTime, &currentTime) == 0)
#endif
        {
            FILE* pOutputFile = s_outputFile.get();
            if (pOutputFile)
            {
#ifdef PLATFORM_UNIX
                char szFormatted[MAX_PATH];
                strftime(szFormatted, MAX_PATH, szFormat, &localTime);
                fprintf(pOutputFile, szFormatted);
#else
                WCHAR szFormatted[MAX_PATH];
                wcsftime(szFormatted, MAX_PATH, szFormat, &localTime);
                fwprintf(pOutputFile, szFormatted);
#endif
            }
        }
    }
}

//static
void MicrosoftInstrumentationEngine::CLogging::OutputLogFileLine(_In_ const WCHAR* szLine)
{
    FILE* pOutputFile = s_outputFile.get();
    if (pOutputFile)
    {
        fwprintf(pOutputFile, szLine);
        fwprintf(pOutputFile, _T("\n"));
        fflush(pOutputFile);
    }
}

//static
void MicrosoftInstrumentationEngine::CLogging::CloseLogFile()
{
    FILE* pOutputFile = s_outputFile.get();
    if (pOutputFile)
    {
        s_outputFile.reset(nullptr);

        fflush(pOutputFile);
        fclose(pOutputFile);
    }
}

// Call this to determine if logging should be allowed for a specific log type.
// static
bool MicrosoftInstrumentationEngine::CLogging::AllowLogEntry(_In_ LoggingFlags flags)
{
    if (!s_initialize.IsSuccessful())
    {
        return false;
    }

    return IsFlagSet(s_loggingFlags, flags) || IsFlagSet(s_enableLoggingToFile, flags);
}

// static
HRESULT MicrosoftInstrumentationEngine::CLogging::Initialize()
{
    HRESULT hr = s_initialize.Get();
    if (SUCCEEDED(hr))
    {
        InterlockedIncrement(&s_usageCount);
    }
    return hr;
}

// static
HRESULT MicrosoftInstrumentationEngine::CLogging::InitializeCore()
{
    HRESULT hr = S_OK;

    WCHAR wszLogLevel[MAX_PATH];
    ZeroMemory(wszLogLevel, MAX_PATH);
    bool fHasLogLevel = GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_LogLevel"), wszLogLevel, MAX_PATH) > 0;

    if (fHasLogLevel)
    {
        // Only want to support error logging in the event log, so specify that the allowed flags is Errors.
        // Currently, downstream dependencies also log only when this flag is set.
        LoggingFlags loggingType = ExtractLoggingFlags(wszLogLevel, LoggingFlags_Errors);

        if (loggingType != LoggingFlags_None)
        {
            if (FAILED(hr = CLogging::InitializeEventLogging()))
            {
                return hr;
            }
            if (FAILED(hr = CLogging::SetLoggingFlagsCore(loggingType)))
            {
                return hr;
            }
        }
    }

    WCHAR wszFileLogLevel[MAX_PATH];
    ZeroMemory(wszFileLogLevel, MAX_PATH);
    bool fHasFileLogLevel = GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_FileLog"), wszFileLogLevel, MAX_PATH) > 0;

    WCHAR wszFileLogPath[MAX_PATH];
    ZeroMemory(wszFileLogPath, MAX_PATH);
    bool fHasFileLogPath = GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_FileLogPath"), wszFileLogPath, MAX_PATH) > 0;

    // Determine from where the file logging flags should be parsed.
    // File logging supports all log levels, so specify that the allowed flags is All.
    // File logging is enabled under two scenarios:
    // 1) If the FileLog env var is specified and it has a value that is something other than None, when parsed.
    // 2) If the LogLevel env var AND FileLogPath env var are specified and the LogLevel value is something other than None, when parsed.
    LoggingFlags fileLoggingFlags = LoggingFlags_None;
    if (fHasFileLogLevel)
    {
        fileLoggingFlags = ExtractLoggingFlags(wszFileLogLevel, LoggingFlags_All);
    }
    else if (fHasLogLevel && fHasFileLogPath)
    {
        fileLoggingFlags = ExtractLoggingFlags(wszLogLevel, LoggingFlags_All);
    }

    // Enable file logging
    if (fileLoggingFlags != LoggingFlags_None)
    {
        CLogging::EnableLoggingToFileCore(fileLoggingFlags, wszFileLogPath);
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CLogging::Shutdown()
{
    if (!s_initialize.IsSuccessful())
    {
        return E_UNEXPECTED;
    }

    if (0 == InterlockedDecrement(&s_usageCount))
    {
        return TerminateEventLogging();
    }
    return S_OK;
}

// static
LoggingFlags MicrosoftInstrumentationEngine::CLogging::ExtractLoggingFlags(
    _In_ LPCWSTR wszRequestedFlagNames,
    _In_ LoggingFlags allowedFlags
)
{
    if (nullptr == wszRequestedFlagNames)
    {
        return LoggingFlags_None;
    }

    // If the request is for all logging flags, just return the allowable flags
    if (wcsstr(wszRequestedFlagNames, _T("All")) != nullptr)
    {
        return allowedFlags;
    }

    // For each logging flag, check that the named flag was specified and that the log flag is something
    // that we are actually trying to find. Combine the results of each (since each result will be a
    // single flag) and return the combination.
    return (LoggingFlags)(
        ExtractLoggingFlag(wszRequestedFlagNames, allowedFlags, _T("Errors"), LoggingFlags_Errors) |
        ExtractLoggingFlag(wszRequestedFlagNames, allowedFlags, _T("Messages"), LoggingFlags_Trace) |
        ExtractLoggingFlag(wszRequestedFlagNames, allowedFlags, _T("Dumps"), LoggingFlags_InstrumentationResults)
        );
}

// static
LoggingFlags MicrosoftInstrumentationEngine::CLogging::ExtractLoggingFlag(
    _In_ LPCWSTR wszRequestedFlagNames,
    _In_ LoggingFlags allowedFlags,
    _In_ LPCWSTR wszSingleTestFlagName,
    _In_ LoggingFlags singleTestFlag
)
{
    if (nullptr == wszRequestedFlagNames || nullptr == wszSingleTestFlagName)
    {
        return LoggingFlags_None;
    }

    // Test that the desired flag is in the combination of flags that are allowed to be checked.
    // Additionally, check that the name of the flag is in the string that specifies what types
    // of messages are requested to be logged.
    if (IsFlagSet(allowedFlags, singleTestFlag) && wcsstr(wszRequestedFlagNames, wszSingleTestFlagName) != nullptr)
    {
        return singleTestFlag;
    }
    return LoggingFlags_None;
}

MicrosoftInstrumentationEngine::CLogging::XmlDumpHelper::XmlDumpHelper(const WCHAR* tag, const unsigned int indent)
{
    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }
    m_tag = tag;

    for (unsigned int i = 0; i < indent; ++i)
    {
        m_indent += _T("    ");
    }
    m_childrenIndent = m_indent + _T("    ");
    m_result << m_indent << _T("<") << m_tag << _T(">") << _T("\r\n");
}
MicrosoftInstrumentationEngine::CLogging::XmlDumpHelper::~XmlDumpHelper()
{
    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }

    m_result << m_indent << _T("</") << m_tag << _T(">") << _T("\r\n") << std::flush;

    CLogging::LogDumpMessage(m_result.str().c_str());
}

void MicrosoftInstrumentationEngine::CLogging::XmlDumpHelper::WriteStringNode(const WCHAR* name, const WCHAR* value)
{
    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }
    m_result << m_childrenIndent << _T("<") << name << _T(">") << value << _T("</") << name << _T(">") << _T("\r\n");
}

void MicrosoftInstrumentationEngine::CLogging::XmlDumpHelper::WriteUlongNode(const WCHAR* name, const ULONG32 value)
{
    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }

    m_result << showbase // show the 0x prefix
        << internal // fill between the prefix and the number
        << setfill(_T('0')); // fill with 0s

    m_result << m_childrenIndent << _T('<') << name << _T('>') << hex << setw(10) << value << _T("</") << name << _T('>') << _T("\r\n");
}
