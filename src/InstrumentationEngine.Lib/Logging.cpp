// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "Logging.h"
#include "../InstrumentationEngine.Api/InstrumentationEngine.h"

using namespace std;

CInitOnce CLogging::s_initialize([]() { return InitializeCore(); });
CSingleton<CLoggerService> CLogging::s_loggerService;
atomic_size_t CLogging::s_initCount(0);
atomic<LoggingFlags> CLogging::s_cachedFlags(LoggingFlags_None);

namespace {
    tstring FormatHex(ULONG32 value)
    {
        constexpr size_t width = 10;

        tstring result(width, _T('0'));
        result[1] = _T('x');

        size_t pos = width - 1;
        while (value != 0) {
            const auto nibble = value & 0xf;
            if (nibble < 0xa)
                result[pos] = _T('0') + nibble;
            else
                result[pos] = _T('a') + nibble - 0xa;
            --pos;
            value >>= 4;
        }

        return result;
    }
}

// static
bool CLogging::AllowLogEntry(_In_ LoggingFlags flags)
{
    IfNotInitRetFalse(s_initialize);

    return s_loggerService.Get()->AllowLogEntry(flags);
}

// static
HRESULT CLogging::GetLoggingFlags(_Out_ LoggingFlags* pLoggingFlags)
{
    IfNotInitRetUnexpected(s_initialize);

    IfNullRetPointerNoLog(pLoggingFlags);

    *pLoggingFlags = s_cachedFlags;
    return S_OK;
}

// static
HRESULT CLogging::UpdateInstrumentationMethodLoggingFlags(_In_ GUID classId, _In_ LoggingFlags loggingFlags)
{
    IfNotInitRetUnexpected(s_initialize);

    return s_loggerService.Get()->UpdateInstrumentationMethodLoggingFlags(classId, loggingFlags);
}

// static
void CLogging::LogMessage(_In_ const WCHAR* wszMessage, ...)
{
    IfNotInitRet(s_initialize);

    LoggingFlags flag;
    if (SUCCEEDED(GetLoggingFlags(&flag)) && (flag & LoggingFlags_Trace) != 0)
    {
        va_list argptr;
        va_start(argptr, wszMessage);
        VLogMessage(wszMessage, argptr);
        va_end(argptr);
    }
}

// static
void CLogging::LogError(_In_ const WCHAR* wszError, ...)
{
    IfNotInitRet(s_initialize);

    LoggingFlags flag;
    if (SUCCEEDED(GetLoggingFlags(&flag)) && (flag & LoggingFlags_Errors) != 0)
    {
        va_list argptr;
        va_start(argptr, wszError);
        VLogError(wszError, argptr);
        va_end(argptr);
    }
}

// static
void CLogging::LogDumpMessage(_In_ const WCHAR* wszMessage, ...)
{
    IfNotInitRet(s_initialize);

    LoggingFlags flag;
    if (SUCCEEDED(GetLoggingFlags(&flag)) && (flag & LoggingFlags_InstrumentationResults) != 0)
    {
        va_list argptr;
        va_start(argptr, wszMessage);
        VLogDumpMessage(wszMessage, argptr);
        va_end(argptr);
    }
}

// static
void CLogging::VLogMessage(_In_ const WCHAR* wszMessage, _In_ va_list argptr)
{
    IfNotInitRet(s_initialize);
    s_loggerService.Get()->LogMessage(wszMessage, argptr);
}

// static
void CLogging::VLogError(_In_ const WCHAR* wszError, _In_ va_list argptr)
{
    IfNotInitRet(s_initialize);
    s_loggerService.Get()->LogError(wszError, argptr);
}

// static
void CLogging::VLogDumpMessage(_In_ const WCHAR* wszMessage, _In_ va_list argptr)
{
    IfNotInitRet(s_initialize);
    s_loggerService.Get()->LogDumpMessage(wszMessage, argptr);
}

// static
HRESULT CLogging::Initialize()
{
    HRESULT hr = s_initialize.Get();

    if (s_initialize.IsSuccessful())
    {
        ++s_initCount;
    }

    return hr;
}

// static
HRESULT CLogging::InitializeCore()
{
    return s_loggerService.Get()->Initialize(OnLoggingFlagsUpdated);
}

// static
void CLogging::SetLogToDebugPort(_In_ bool enable)
{
    IfNotInitRet(s_initialize);

    s_loggerService.Get()->SetLogToDebugPort(enable);
}

// static
HRESULT CLogging::SetLoggingFlags(_In_ LoggingFlags loggingFlags)
{
    IfNotInitRetUnexpected(s_initialize);

    return s_loggerService.Get()->SetLoggingFlags(loggingFlags);
}

// static
HRESULT CLogging::SetLoggingHost(_In_opt_ IProfilerManagerLoggingHost* pLoggingHost)
{
    IfNotInitRetUnexpected(s_initialize);

    return s_loggerService.Get()->SetLoggingHost(pLoggingHost);
}

// static
HRESULT CLogging::Shutdown()
{
    IfNotInitRetUnexpected(s_initialize);

    if (0 == --s_initCount)
    {
        HRESULT hr = S_OK;
        IfFailRetNoLog(s_loggerService.Get()->Shutdown());
    }
    return S_OK;
}

// static
HRESULT CLogging::SetLogFilePath(_In_ LPCWSTR wszLogFilePath)
{
    IfNotInitRetUnexpected(s_initialize);

    return s_loggerService.Get()->SetLogFilePath(wszLogFilePath);
}

// static
HRESULT CLogging::SetLogFileLevel(_In_ LoggingFlags fileLogFlags)
{
    IfNotInitRetUnexpected(s_initialize);

    return s_loggerService.Get()->SetLogFileLevel(fileLogFlags);
}

// static
void CLogging::OnLoggingFlagsUpdated(_In_ const LoggingFlags& flags)
{
    s_cachedFlags = flags;
}

CLogging::XmlDumpHelper::XmlDumpHelper(const WCHAR* tag, const unsigned int indent)
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
    m_result += m_indent + _T("<") + m_tag + _T(">") + _T("\r\n");
}

CLogging::XmlDumpHelper::~XmlDumpHelper()
{
    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }

    m_result += m_indent + _T("</") + m_tag + _T(">") + _T("\r\n");

    CLogging::LogDumpMessage(_T("%s"), m_result.c_str());
}

void CLogging::XmlDumpHelper::WriteStringNode(const WCHAR* name, const WCHAR* value)
{
    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }
    m_result += m_childrenIndent + _T("<") + name + _T(">") + value + _T("</") + name + _T(">") + _T("\r\n");
}

void CLogging::XmlDumpHelper::WriteUlongNode(const WCHAR* name, const ULONG32 value)
{
    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }

    m_result += m_childrenIndent + _T('<') + name + _T('>') + FormatHex(value) + _T("</") + name + _T('>') + _T("\r\n");
}

// Implemenation of Assert functions found in Macros.h
void AssertLogFailure(_In_ const WCHAR* wszError, ...)
{
    LoggingFlags flag;
    if (SUCCEEDED(CLogging::GetLoggingFlags(&flag)) && (flag & LoggingFlags_Errors) != 0)
    {
        va_list argptr;
        va_start(argptr, wszError);
        CLogging::VLogError(wszError, argptr);
        va_end(argptr);
    }
}
