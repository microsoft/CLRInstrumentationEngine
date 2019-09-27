// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// CExtensionsHost.cpp : Implementation of CExtensionsHost

#include "stdafx.h"

#ifndef PLATFORM_UNIX

#include "../ExtensionsCommon/ModuleUtils.h"
#include "../ExtensionsCommon/PathUtils.h"
#include "../ExtensionsCommon/TextUtils.h"

#include "../ExtensionsCommon/AgentValidation.h"
#include "../ExtensionsCommon/Environment.h"

#include "RawProfilerHookLoader.h"
#include "RawProfilerHookSettingsReader.h"
#include "../Common.Lib/SafeFindFileHandle.h"

using Agent::Diagnostics::Param;

#endif

#include "CExtensionsHost.h"

using namespace CommonLib;

std::wstring configFilePattern = L"*.config";

// CExtensionsHost

#ifndef PLATFORM_UNIX
_Check_return_ HRESULT CExtensionsHost::InternalInitialize(
    _In_ const IProfilerManagerSptr& spProfilerManager)
{
    IProfilerManagerLoggingSptr spLogger = NULL;
    IfFailRet(spProfilerManager->GetLoggingInstance(&spLogger));
    this->SetLogger(spLogger);

    std::vector<ATL::CComBSTR> configFiles;

    IfFailRet(Agent::Settings::CEnvironment::GetConfigurationPaths(configFiles, spLogger));

    // Check immediate folder if config files were not found.
    if (0 == configFiles.size())
    {
        auto hModule = Agent::Io::ModuleUtils::GetHostModule();
        IfTrueRet(nullptr == hModule, HRESULT_FROM_WIN32(::GetLastError()));

        const std::wstring strModuleFolder = Agent::Io::ModuleUtils::GetModuleDirectory(hModule);
        IfTrueRet(Agent::Text::TextUtils::IsEmpty(strModuleFolder), E_FAIL);

        // Search for files
        const std::wstring strFileSearchPattern = Agent::Io::PathUtils::Combine(
            strModuleFolder,
            configFilePattern);

        WIN32_FIND_DATA findFileData;
        SafeFindFileHandle hSearchHandle = FindFirstFile(strFileSearchPattern.c_str(), &findFileData);
        if (hSearchHandle != INVALID_HANDLE_VALUE)
        {
            do
            {
                // Skip any directories; we only want files
                if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                {
                    continue;
                }

                configFiles.push_back(
                    ATL::CComBSTR(
                        Agent::Io::PathUtils::Combine(
                            strModuleFolder,
                            findFileData.cFileName).c_str()));

            } while (FindNextFile(hSearchHandle, &findFileData));
        }
    }

    if (configFiles.size() > 0)
    {
        IfFailRet(spProfilerManager->SetupProfilingEnvironment(&configFiles[0], static_cast<UINT>(configFiles.size())));
    }
    else
    {
        TraceError(_T("No Instrumentation method configs found to load in process %u."), GetCurrentProcessId());
    }

    Agent::Host::CRawProfilerHookSettingsReader rawHookSettingsReader;
    IfFailRet(BuildUpObjRef(rawHookSettingsReader));

    GUID clsidRAWProfilerHook = { 0 };
    std::wstring strRawProfilerHookModulePath;
    HRESULT hr = S_OK;
    IfFailRetHresult(
        rawHookSettingsReader.ReadSettings(
            clsidRAWProfilerHook,
            strRawProfilerHookModulePath), hr);

    if (S_FALSE != hr)
    {
        TraceMsg(
            L"Attempting to load raw profiler hook",
            Param(L"path", strRawProfilerHookModulePath));

        IfFalseRet(Agent::Io::PathUtils::Exists(strRawProfilerHookModulePath), E_NOT_SET);

        Agent::Host::CRawProfilerHookLoader rawHookLoader;
        IfFailRet(BuildUpObjRef(rawHookLoader));

        IUnknownSptr spRawProfilerHook;

        IfFailRet(
            rawHookLoader.LoadRawProfilerHookComponentFrom(
            strRawProfilerHookModulePath,
            clsidRAWProfilerHook,
            spRawProfilerHook,
            m_hRawHookModule));

        TraceMsg(L"Raw profiler hook module loaded, component instance created");

        IfFailRet(spProfilerManager->AddRawProfilerHook(spRawProfilerHook));
    }
    else
    {
        TraceMsg(L"Raw profiler hook module is not specified, skip loading.");
    }

    return S_OK;
}

#else

HRESULT CExtensionsHost::Initialize(
    _In_ IProfilerManager* pProfilerManager
)
{
    HRESULT hr = S_OK;

    CComPtr<IProfilerManagerLogging> pLogger;
    IfFailRet(pProfilerManager->GetLoggingInstance(&pLogger));

#ifdef X86
    WCHAR wszProfilerPathVariableName[] = _T("CORECLR_PROFILER_PATH_32");
#else
    WCHAR wszProfilerPathVariableName[] = _T("CORECLR_PROFILER_PATH_64");
#endif

    WCHAR wszProfilerPath[MAX_PATH];
    if (!GetEnvironmentVariable(wszProfilerPathVariableName, wszProfilerPath, MAX_PATH))
    {
        return E_UNEXPECTED;
    }

    WCHAR* pFileName = PathFindFileName(wszProfilerPath);
    if (pFileName == wszProfilerPath)
    {
        return E_UNEXPECTED;
    }
    *pFileName = _T('\0');

#ifdef X86
    WCHAR wszConfigName[] = _T("ProductionBreakpoints_x86.config");
#else
    WCHAR wszConfigName[] = _T("ProductionBreakpoints_x64.config");
#endif

    StringCchCatW(wszProfilerPath, MAX_PATH, wszConfigName);

    CComBSTR bstrConfigPath = wszProfilerPath;
    IfFailRet(pProfilerManager->SetupProfilingEnvironment(&bstrConfigPath, 1));

    return S_OK;
}

#endif