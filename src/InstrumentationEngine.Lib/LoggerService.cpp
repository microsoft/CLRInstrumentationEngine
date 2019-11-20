// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "LoggerService.h"
#include "../InstrumentationEngine.Api/InstrumentationEngine.h"

#ifndef PLATFORM_UNIX
#include "DebugLoggerSink.h"
#include "EventLoggerSink.h"
#endif
#include "FileLoggerSink.h"
#include "HostLoggerSink.h"

using namespace MicrosoftInstrumentationEngine;
using namespace std;

// Need to define this to use GUID in a set
bool operator< (const GUID& guid1, const GUID& guid2)
{
    if (guid1.Data1 != guid2.Data1)
        return (guid1.Data1 < guid2.Data1);

    if (guid1.Data2 != guid2.Data2)
        return (guid1.Data2 < guid2.Data2);

    if (guid1.Data3 != guid2.Data3)
        return (guid1.Data3 < guid2.Data3);

    for (int i = 0; i < 8; i++)
        if (guid1.Data4[i] != guid2.Data4[i])
            return (guid1.Data4[i] < guid2.Data4[i]);

    // They are equal, so not less_then
    return false;
}

CLoggerService::CLoggerService() :
    m_defaultFlags(LoggingFlags_None),
    m_effectiveFlags(LoggingFlags_None),
    m_instrumentationMethodFlags(LoggingFlags_None),
    m_fLogToDebugPort(false),
    m_loggingFlagsToInstrumentationMethodsMap({
        { LoggingFlags_Errors,                 {} },
        { LoggingFlags_Trace,                  {} },
        { LoggingFlags_InstrumentationResults, {} },
    }),
    m_initialize([=]() { return InitializeCore(); })
{
}

CLoggerService::~CLoggerService()
{
}

HRESULT CLoggerService::GetLoggingHost(_Out_ IProfilerManagerLoggingHost** ppLoggingHost)
{
    IfNullRetPointerNoLog(ppLoggingHost);

    *ppLoggingHost = nullptr;

    IfNotInitRetUnexpected(m_initialize);

    CCriticalSectionHolder holder(&m_cs);

    return m_pLoggingHost.CopyTo(ppLoggingHost);
}

HRESULT CLoggerService::SetLoggingHost(_In_ IProfilerManagerLoggingHost* pLoggingHost)
{
    IfNotInitRetUnexpected(m_initialize);

    CCriticalSectionHolder holder(&m_cs);

    m_pLoggingHost = pLoggingHost;

    return RecalculateLoggingFlags();
}

HRESULT CLoggerService::GetLoggingFlags(_Out_ LoggingFlags* pLoggingFlags)
{
    HRESULT hr = S_OK;

    IfNullRetPointerNoLog(pLoggingFlags);

    *pLoggingFlags = LoggingFlags_None;

    IfNotInitRetUnexpected(m_initialize);

    CCriticalSectionHolder holder(&m_cs);

    // Intentionally returning the effective flags instead of the default flags
    // (ones set by default or by SetLoggingFlags). The GetLoggingFlags method
    // is primarily meant for consumers external of the Instrumentation Engine
    // assembly to determine which flags are supported rather than literally
    // reporting what the default was or what was set by SetLoggingFlags.
    *pLoggingFlags = m_effectiveFlags;

    return S_OK;
}

HRESULT CLoggerService::UpdateInstrumentationMethodLoggingFlags(_In_ GUID classId, _In_ LoggingFlags loggingFlags)
{
    HRESULT hr = S_OK;

    IfNotInitRetUnexpected(m_initialize);

    CCriticalSectionHolder holder(&m_cs);

    m_instrumentationMethodFlags = LoggingFlags_None;

    IfFailRetNoLog(UpdateInstrumentationMethodFlags(classId, loggingFlags));

    // Updates m_instrumentationMethodFlags with any changes to the supported LoggingFlags has at least one InstrumentationMethod
    // requiring logging for that level.
    for (unordered_map<LoggingFlags, set<GUID>>::iterator it = m_loggingFlagsToInstrumentationMethodsMap.begin();
        it != m_loggingFlagsToInstrumentationMethodsMap.end();
        it++)
    {
        if (!(it->second.empty()))
        {
            m_instrumentationMethodFlags = (LoggingFlags)(m_instrumentationMethodFlags | it->first);
        }
    }

    return RecalculateLoggingFlags();
}

HRESULT CLoggerService::SetLoggingFlags(_In_ LoggingFlags loggingFlags)
{
    IfNotInitRetUnexpected(m_initialize);

    CCriticalSectionHolder holder(&m_cs);

    m_defaultFlags = loggingFlags;

    return RecalculateLoggingFlags();
}

bool CLoggerService::GetLogToDebugPort()
{
    IfNotInitRetFalse(m_initialize);

    return m_fLogToDebugPort;
}

void CLoggerService::SetLogToDebugPort(_In_ bool enable)
{
    IfNotInitRet(m_initialize);

    CCriticalSectionHolder holder(&m_cs);

    m_fLogToDebugPort = enable;

    RecalculateLoggingFlags();
}

HRESULT CLoggerService::Initialize()
{
    return m_initialize.Get();
}

HRESULT CLoggerService::InitializeCore()
{
    HRESULT hr = S_OK;

    // Get the general logging level from the environment variable
    WCHAR wszLogLevel[MAX_PATH];
    ZeroMemory(wszLogLevel, MAX_PATH);
    if (GetEnvironmentVariable(LogLevelEnvironmentVariableName, wszLogLevel, MAX_PATH) > 0)
    {
        m_defaultFlags = ExtractLoggingFlags(wszLogLevel);
    }

    // Get the list of sinks to which logging shall be sent
    if (FAILED(CreateSinks(m_allSinks)))
    {
        return hr;
    }

    // Initialize each sink with the current logger service
    for (shared_ptr<ILoggerSink>& pSink : m_allSinks)
    {
        IfFailRetNoLog(pSink->Initialize(this));
    }

    return RecalculateLoggingFlags();
}

void CLoggerService::LogMessage(_In_ LPCWSTR wszMessage, _In_ va_list argptr)
{
    IfNotInitRet(m_initialize);

    CCriticalSectionHolder holder(&m_cs);

    // Check if messages are allowed
    if (!AllowLogEntry(LoggingFlags_Trace))
    {
        return;
    }

    // Format and truncate the message
    WCHAR szLogEntry[LogEntryMaxSize];
    _vsnwprintf_s(szLogEntry, LogEntryMaxSize, _TRUNCATE, wszMessage, argptr);

    // Send formatted message to each sink
    for (shared_ptr<ILoggerSink>& pSink : m_messageSinks)
    {
        pSink->LogMessage(szLogEntry);
    }
}

void CLoggerService::LogMessage(_In_ LPCWSTR wszMessage, ...)
{
    va_list argptr;
    va_start (argptr, wszMessage);
    LogMessage(wszMessage, argptr);
    va_end (argptr);
}

void CLoggerService::LogError(_In_ LPCWSTR wszError, _In_ va_list argptr)
{
    IfNotInitRet(m_initialize);

    CCriticalSectionHolder holder(&m_cs);

    // Check if errors are allowed
    if (!AllowLogEntry(LoggingFlags_Errors))
    {
        return;
    }

    // Format and truncate the error
    WCHAR szLogEntry[LogEntryMaxSize];
    _vsnwprintf_s(szLogEntry, LogEntryMaxSize, _TRUNCATE, wszError, argptr);

    // Send formatted error to each sink
    for (shared_ptr<ILoggerSink>& pSink : m_errorSinks)
    {
        pSink->LogError(szLogEntry);
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

void CLoggerService::LogError(_In_ LPCWSTR wszError, ...)
{
    va_list argptr;
    va_start(argptr, wszError);
    LogError(wszError, argptr);
    va_end(argptr);
}

void CLoggerService::LogDumpMessage(_In_ LPCWSTR wszMessage, _In_ va_list argptr)
{
    IfNotInitRet(m_initialize);

    CCriticalSectionHolder holder(&m_cs);

    // Check if dumps are allowed
    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }

    // Format and truncate the message
    WCHAR szLogEntry[LogEntryMaxSize];
    _vsnwprintf_s(szLogEntry, LogEntryMaxSize, _TRUNCATE, wszMessage, argptr);

    // Send formatted message to each sink
    for (shared_ptr<ILoggerSink>& pSink : m_dumpSinks)
    {
        pSink->LogDumpMessage(szLogEntry);
    }
}

void CLoggerService::LogDumpMessage(_In_ LPCWSTR wszMessage, ...)
{
    va_list argptr;
    va_start(argptr, wszMessage);
    LogDumpMessage(wszMessage, argptr);
    va_end(argptr);
}

bool CLoggerService::AllowLogEntry(_In_ LoggingFlags flags)
{
    IfNotInitRetFalse(m_initialize);

    return IsFlagSet((LoggingFlags)(m_effectiveFlags | m_instrumentationMethodFlags), flags);
}

// static
LoggingFlags CLoggerService::ExtractLoggingFlags(_In_ LPCWSTR wszRequestedFlagNames)
{
    if (nullptr == wszRequestedFlagNames)
    {
        return LoggingFlags_None;
    }

    // If the request is for all logging flags, just return the allowable flags
    if (wcsstr(wszRequestedFlagNames, _T("All")) != nullptr)
    {
        return LoggingFlags_All;
    }

    // For each logging flag, check that the named flag was specified. Combine the results
    // of each (since each result will be a single flag) and return the combination.
    return (LoggingFlags)(
        ExtractLoggingFlag(wszRequestedFlagNames, _T("Errors"), LoggingFlags_Errors) |
        ExtractLoggingFlag(wszRequestedFlagNames, _T("Messages"), LoggingFlags_Trace) |
        ExtractLoggingFlag(wszRequestedFlagNames, _T("Dumps"), LoggingFlags_InstrumentationResults)
        );
}

// static
LoggingFlags CLoggerService::ExtractLoggingFlag(
    _In_ LPCWSTR wszRequestedFlagNames,
    _In_ LPCWSTR wszSingleTestFlagName,
    _In_ LoggingFlags singleTestFlag
)
{
    if (nullptr == wszRequestedFlagNames || nullptr == wszSingleTestFlagName)
    {
        return LoggingFlags_None;
    }

    // Check that the name of the flag is in the string that specifies what types
    // of messages are requested to be logged.
    if (wcsstr(wszRequestedFlagNames, wszSingleTestFlagName) != nullptr)
    {
        return singleTestFlag;
    }

    return LoggingFlags_None;
}

HRESULT CLoggerService::RecalculateLoggingFlags()
{
    HRESULT hr = S_OK;

    m_dumpSinks.clear();
    m_errorSinks.clear();
    m_messageSinks.clear();

    LoggingFlags effectiveFlags = LoggingFlags_None;
    for (shared_ptr<ILoggerSink>& pSink : m_allSinks)
    {
        // Reset the sink; get its desired logging level
        LoggingFlags sinkFlags;
        LoggingFlags instrumentationMethodSinkFlags;
        IfFailRetNoLog(pSink->Reset(m_defaultFlags, &sinkFlags));
        IfFailRetNoLog(pSink->Reset(m_instrumentationMethodFlags, &instrumentationMethodSinkFlags));

        // Add the sink to each logging level vector
        if (IsFlagSet(sinkFlags | instrumentationMethodSinkFlags, LoggingFlags_Errors))
        {
            m_errorSinks.push_back(pSink);
        }
        if (IsFlagSet(sinkFlags | instrumentationMethodSinkFlags, LoggingFlags_InstrumentationResults))
        {
            m_dumpSinks.push_back(pSink);
        }
        if (IsFlagSet(sinkFlags | instrumentationMethodSinkFlags, LoggingFlags_Trace))
        {
            m_messageSinks.push_back(pSink);
        }

        // Aggregate all logging levels together to get the overall logging level.
        effectiveFlags = static_cast<LoggingFlags>(effectiveFlags | sinkFlags);
    }

    m_effectiveFlags = effectiveFlags;

    return S_OK;
}

HRESULT CLoggerService::Shutdown()
{
    CCriticalSectionHolder holder(&m_cs);

    HRESULT hr = S_OK;
    for (shared_ptr<ILoggerSink>& pSink : m_allSinks)
    {
        IfFailRetNoLog(pSink->Shutdown());
    }

    m_allSinks.clear();
    m_dumpSinks.clear();
    m_errorSinks.clear();
    m_messageSinks.clear();

    m_pLoggingHost.Release();

    m_initialize.Reset();

    return S_OK;
}

HRESULT CLoggerService::CreateSinks(vector<shared_ptr<ILoggerSink>>& sinks)
{
#ifndef PLATFORM_UNIX
    sinks.push_back(make_shared<CDebugLoggerSink>());
    sinks.push_back(make_shared<CEventLoggerSink>());
#endif
    sinks.push_back(make_shared<CFileLoggerSink>());
    sinks.push_back(make_shared<CHostLoggerSink>());

    return S_OK;
}

HRESULT CLoggerService::UpdateInstrumentationMethodFlags(_In_ GUID classId, _In_ LoggingFlags loggingFlags)
{
    HRESULT hr = S_OK;

    IfFailRetNoLog(UpdateInstrumentationMethodFlagsInternal(
        classId,
        loggingFlags,
        LoggingFlags_Errors));

    IfFailRetNoLog(UpdateInstrumentationMethodFlagsInternal(
        classId,
        loggingFlags,
        LoggingFlags_Trace));

    IfFailRetNoLog(UpdateInstrumentationMethodFlagsInternal(
        classId,
        loggingFlags,
        LoggingFlags_InstrumentationResults));

    return S_OK;
}

HRESULT CLoggerService::UpdateInstrumentationMethodFlagsInternal(_In_ GUID classId, _In_ LoggingFlags loggingFlags, _In_ LoggingFlags loggingLevel)
{
    set<GUID>* pSet = &(m_loggingFlagsToInstrumentationMethodsMap[loggingLevel]);

    bool exists = pSet->find(classId) != pSet->end();
    bool shouldExist = (loggingLevel & loggingFlags) != 0;
    if (exists && !shouldExist)
    {
        // Remove
        size_t result = pSet->erase(classId);
        if (result < 1)
        {
            // Unable to remove
            return E_FAIL;
        }
    }
    else if (!exists && shouldExist)
    {
        // Add
        std::pair<set<GUID>::iterator, bool> ret = pSet->emplace(classId);
        if (!ret.second)
        {
            // Unable to insert
            return E_FAIL;
        }
    }

    return S_OK;
}
