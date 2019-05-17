#include "stdafx.h"
#include "FileLoggerSink.h"

using namespace MicrosoftInstrumentationEngine;

CFileLoggerSink::CFileLoggerSink() :
    m_fileFlags(LoggingFlags_None),
    m_pOutputFile(nullptr)
{
}

CFileLoggerSink::~CFileLoggerSink()
{
    CloseLogFile();
}

HRESULT CFileLoggerSink::Initialize(_In_ CLoggerService* pLogging)
{
    WCHAR wszFileLogLevel[MAX_PATH];
    ZeroMemory(wszFileLogLevel, MAX_PATH);
    if (GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_FileLog"), wszFileLogLevel, MAX_PATH) > 0)
    {
        m_fileFlags = CLoggerService::ExtractLoggingFlags(wszFileLogLevel);
    }

    WCHAR wszFileLogPath[MAX_PATH];
    ZeroMemory(wszFileLogPath, MAX_PATH);
    if (GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_FileLogPath"), wszFileLogPath, MAX_PATH) > 0)
    {
        m_tsFilePath = wszFileLogPath;
    }

    return S_OK;
}

void CFileLoggerSink::LogMessage(_In_ LPCWSTR wszMessage)
{
    WritePrefix(LoggingFlags_Trace);
    WriteLine(wszMessage);
}

void CFileLoggerSink::LogError(_In_ LPCWSTR wszMessage)
{
    WritePrefix(LoggingFlags_Trace);
    WriteLine(wszMessage);
}

void CFileLoggerSink::LogDumpMessage(_In_ LPCWSTR wszMessage)
{
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
    LoggingFlags effectiveFlags = m_fileFlags;
    if (LoggingFlags_None == effectiveFlags)
    {
        effectiveFlags = defaultFlags;
    }

    // Start logging to file if logging has been enabled (has flags, not already enabled, and has file path).
    if (LoggingFlags_None != effectiveFlags && !m_pOutputFile && !m_tsFilePath.empty())
    {
        // Do not use IfFailRet (or CLogger) as it will cause a deadlock
        HRESULT hr;
        if (FAILED(hr = InitializeLogFile()))
        {
            return hr;
        }
    }

    *pEffectiveFlags = effectiveFlags;

    return S_OK;
}

HRESULT CFileLoggerSink::Shutdown()
{
    CloseLogFile();
    return S_OK;
}

void CFileLoggerSink::SetFlags(_In_ LoggingFlags fileFlags)
{
    m_fileFlags = fileFlags;
}

void CFileLoggerSink::SetFilePath(_In_ const tstring& tsFilePath)
{
    m_tsFilePath = tsFilePath;
}

const tstring CFileLoggerSink::GetEffectiveFilePath()
{
    return m_tsFilePathActual;
}

void CFileLoggerSink::CloseLogFile()
{
    FILE* pOutputFile = m_pOutputFile.get();
    if (pOutputFile)
    {
        m_pOutputFile.reset(nullptr);

        fflush(pOutputFile);
        fclose(pOutputFile);
    }
}

HRESULT CFileLoggerSink::InitializeLogFile()
{
    tstring tsCandidateFilePath(m_tsFilePath);
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
    m_pOutputFile.reset(_wfopen(wszFilePath, _T("a")));
#else
    m_pOutputFile.reset(_wfsopen(wszFilePath, _T("a"), _SH_DENYWR));
#endif

    if (m_pOutputFile)
    {
        m_tsFilePathActual = wszFilePath;
    }

    return S_OK;
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
                fwprintf(pOutputFile, szFormatted);
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