// Copyright (c) Microsoft Corporation. All rights reserved.
// 

// ProfilerManagerHost.cpp : Implementation of CProfilerManagerHost

#include "stdafx.h"
#include "ProfilerManagerHost.h"

//static
CProfilerManagerHost* CProfilerManagerHost::s_instance;

// CProfilerManagerHost
const wchar_t CProfilerManagerHost::HostConfigPathEnvName[] = L"MicrosoftInstrumentationEngine_ConfigPath";
const wchar_t CProfilerManagerHost::TestOutputFileEnvName[] = L"Nagler_TestOutput";
const wchar_t CProfilerManagerHost::TestResultFolder[] = L"TestResults";

HRESULT CProfilerManagerHost::Initialize(_In_ IProfilerManager* pProfilerManager)
{
    wchar_t wszConfigPath[MAX_PATH];
    DWORD dwRes = GetEnvironmentVariableW(HostConfigPathEnvName, wszConfigPath, MAX_PATH);
    if (dwRes == 0)
    {
        ATLASSERT(!L"Failed to get config file path");
        return E_FAIL;
    }

    CComBSTR bstrConfigurationPath = wszConfigPath;

    SetupOutputFile();

    CComQIPtr<IProfilerManagerLogging> pProfilerManagerLogging = pProfilerManager;
    pProfilerManagerLogging->SetLoggingFlags((LoggingFlags)(LoggingFlags_InstrumentationResults | LoggingFlags_Errors));
    pProfilerManager->SetLoggingHost((IProfilerManagerLoggingHost*)this);

    pProfilerManager->SetupProfilingEnvironment(&bstrConfigurationPath, 1);

    return S_OK;
}

HRESULT CProfilerManagerHost::LogMessage(_In_ const WCHAR* wszMessage)
{
    return S_OK;
}

HRESULT CProfilerManagerHost::LogError(_In_ const WCHAR* wszError)
{
    ATLASSERT(wszError);
    return S_OK;
}

HRESULT CProfilerManagerHost::LogDumpMessage(_In_ const WCHAR* wszMessage)
{
    DWORD dwLen = (DWORD)wcslen(wszMessage);

    DWORD cActual = 0;

    //Convert results to Utf-8.
    std::vector<char> buffer(dwLen + 1);
    int written = WideCharToMultiByte(CP_UTF8, 0, wszMessage, dwLen, buffer.data(), (int)buffer.size(), nullptr, nullptr);
    if (written > 0 &&
        wcsncmp(wszMessage, L"[TestIgnore]", 12) != 0)
    {
        WriteFile(m_hOutputFile, buffer.data(), written, &cActual, nullptr);
    }

    return S_OK;
}

HRESULT CProfilerManagerHost::SetupOutputFile()
{
    // Initialize output file
    wchar_t testOutput[MAX_PATH];
    DWORD dwRes = GetEnvironmentVariableW(TestOutputFileEnvName, testOutput, MAX_PATH);
    if (dwRes == 0)
    {
        ATLASSERT(!L"Failed to get test output file name");
        return E_FAIL;
    }

    m_hOutputFile.Attach(CreateFile(testOutput, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));
    if (m_hOutputFile == INVALID_HANDLE_VALUE)
    {
        ATLASSERT(!L"Failed to create test output file");
        return E_FAIL;
    }

    return S_OK;
}