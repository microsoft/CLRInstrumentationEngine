// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "FileLoggerSink.h"

using namespace MicrosoftInstrumentationEngine;

CFileLoggerSink::CFileLoggerSink() :
    m_flags(LoggingFlags_None),
    m_pOutputFile(nullptr)
{
}

CFileLoggerSink::~CFileLoggerSink()
{
    CloseLogFile();
}

HRESULT CFileLoggerSink::Initialize(_In_ CLoggerService* pLogging)
{
    return GetPathAndFlags(&m_tsPathCandidate, &m_flags);
}

void CFileLoggerSink::LogMessage(_In_ LPCWSTR wszMessage)
{
    WritePrefix(LoggingFlags_Trace);
    WriteLine(wszMessage);
}

void CFileLoggerSink::LogError(_In_ LPCWSTR wszMessage)
{
    WritePrefix(LoggingFlags_Errors);
    WriteLine(wszMessage);
}

void CFileLoggerSink::LogDumpMessage(_In_ LPCWSTR wszMessage)
{
    WritePrefix(LoggingFlags_InstrumentationResults);
    WriteLine(wszMessage);
}

HRESULT CFileLoggerSink::Reset(_In_ LoggingFlags defaultFlags, _Out_ LoggingFlags* pEffectiveFlags)
{
    if (!pEffectiveFlags)
    {
        return E_POINTER;
    }

    *pEffectiveFlags = LoggingFlags_None;

    // Use flags from FileLog environment variable first. If not defined, then use the requested flags.
    LoggingFlags effectiveFlags = m_flags;
    if (LoggingFlags_None == effectiveFlags)
    {
        effectiveFlags = defaultFlags;
    }

    // Start logging to file if logging has been enabled (has flags, not already enabled, and has file path).
    if (LoggingFlags_None != effectiveFlags && !m_pOutputFile && !m_tsPathCandidate.empty())
    {
        // Split the provided path in parts
        WCHAR wszDrive[_MAX_DRIVE];
        WCHAR wszDirectory[_MAX_DIR];
        WCHAR wszFile[_MAX_FNAME];
        WCHAR wszExtension[_MAX_EXT];
        _wsplitpath_s(
            m_tsPathCandidate.c_str(),
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
        m_pOutputFile.reset(_wfopen(wszFilePath, _T("a")));
#else
        // will create a new file if there is permission to in the
        // given path, and permissions will be inherited.
        m_pOutputFile.reset(_wfsopen(wszFilePath, _T("a"), _SH_DENYWR)); // lgtm[cpp/world-writable-file-creation]
#endif

        m_tsPathActual.clear();
        if (m_pOutputFile)
        {
            m_tsPathActual = wszFilePath;
        }
    }

    // Only return non-None flags if the output file has been opened.
    if (m_pOutputFile)
    {
        *pEffectiveFlags = effectiveFlags;
    }

    return S_OK;
}

HRESULT CFileLoggerSink::Shutdown()
{
    CloseLogFile();
    return S_OK;
}

void CFileLoggerSink::CloseLogFile()
{
    FILE* pOutputFile = m_pOutputFile.get();
    if (pOutputFile)
    {
        fflush(pOutputFile);

        // Pointer has deleter that will close the file.
        m_pOutputFile.reset(nullptr);
    }
}

void CFileLoggerSink::WritePrefix(_In_ LoggingFlags flags)
{
    // Use UTF-16 on Windows to avoid string encoding conversions from multi-byte to Unicode.
#ifdef PLATFORM_UNIX
    const char* szFormat = nullptr;
#else
    const WCHAR* szFormat = nullptr;
#endif

    switch (flags)
    {
    case LoggingFlags_Errors:
        szFormat = ERROR_PREFIX_FORMAT;
        break;
    case LoggingFlags_Trace:
        szFormat = MESSAGE_PREFIX_FORMAT;
        break;
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
            FILE* pOutputFile = m_pOutputFile.get();
            if (pOutputFile)
            {
#ifdef PLATFORM_UNIX
                char szFormatted[MAX_PATH];
                strftime(szFormatted, MAX_PATH, szFormat, &localTime);
                fprintf(pOutputFile, szFormatted);
#else
                WCHAR szFormatted[MAX_PATH];
                wcsftime(szFormatted, MAX_PATH, szFormat, &localTime);

                // Static analysis will complain that the following line prints a non-constant
                // string, but there are no inputs to the string. Ignore.
                fwprintf(pOutputFile, szFormatted);  // lgtm[cpp/non-constant-format]
#endif
            }
        }
    }
}

void CFileLoggerSink::WriteLine(_In_ LPCWSTR wszMessage)
{
    FILE* pOutputFile = m_pOutputFile.get();
    if (pOutputFile)
    {
        fwprintf(pOutputFile, wszMessage);
        fwprintf(pOutputFile, _T("\n"));
        fflush(pOutputFile);
    }
}

const tstring& CFileLoggerSink::GetPathActual()
{
    return m_tsPathActual;
}

HRESULT CFileLoggerSink::GetPathAndFlags(_Out_ tstring* ptsPath, _Out_ LoggingFlags* pFlags)
{
    if (!ptsPath || !pFlags)
    {
        return E_POINTER;
    }

    ptsPath->clear();
    *pFlags = LoggingFlags_None;

    WCHAR wszFileLogLevel[MAX_PATH];
    ZeroMemory(wszFileLogLevel, MAX_PATH);
    if (GetEnvironmentVariable(LogLevelEnvironmentVariableName, wszFileLogLevel, MAX_PATH) > 0)
    {
        *pFlags = CLoggerService::ExtractLoggingFlags(wszFileLogLevel);
    }

    WCHAR wszFileLogPath[MAX_PATH];
    ZeroMemory(wszFileLogPath, MAX_PATH);
    if (GetEnvironmentVariable(LogPathEnvironmentVariableName, wszFileLogPath, MAX_PATH) > 0)
    {
        *ptsPath = wszFileLogPath;
    }

    return S_OK;
}

HRESULT CFileLoggerSink::SetLogFilePath(_In_ LPCWSTR wszLogFilePath)
{
    m_tsPathCandidate = wszLogFilePath;

    m_pOutputFile = nullptr; // resets the pointer & releases the FILE object
    m_tsPathActual.clear();

    return S_OK;
}

HRESULT CFileLoggerSink::SetLogFileLevel(_In_ LoggingFlags fileLogFlags)
{
    m_flags = fileLogFlags;

    m_pOutputFile = nullptr; // resets the pointer & releases the FILE object
    m_tsPathActual.clear();
 
    return S_OK;
}