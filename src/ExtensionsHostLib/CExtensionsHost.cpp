// Copyright (c) Microsoft Corporation. All rights reserved.
//

// CExtensionsHost.cpp : Implementation of CExtensionsHost

#include "stdafx.h"

#include "../ExtensionsCommon/ModuleUtils.h"
#include "../ExtensionsCommon/PathUtils.h"
#include "../ExtensionsCommon/TextUtils.h"

#include "../ExtensionsCommon/AgentValidation.h"
#include "../ExtensionsCommon/Environment.h"

#include "RawProfilerHookLoader.h"
#include "RawProfilerHookSettingsReader.h"
#include "SafeFindFileHandle.h"

#include "../InstrumentationEngine/ImplQueryInterface.h"
#include "../InstrumentationEngine/refcount.h"

#include "CExtensionsHost.h"

using Agent::Diagnostics::Param;

std::wstring configFilePattern = L"*.config";

// CExtensionsHost

//Sergey Kanzhelev:
// this method is a work around the design that InstrumentationEngine has - it separates log file flags and host tracing flags.
// so for default file tracing expirience that you enable via Environment variables you will not get traces from any of extensions
void SetLoggingFlags(_In_ IProfilerManagerLoggingSptr& spLogger)
{
    wchar_t wszEnvVar[MAX_PATH];
    if (GetEnvironmentVariable(L"MicrosoftInstrumentationEngine_LogLevel", wszEnvVar, MAX_PATH) > 0 ||
        GetEnvironmentVariable(L"MicrosoftInstrumentationEngine_FileLog", wszEnvVar, MAX_PATH) > 0)
    {
        LoggingFlags loggingType = LoggingFlags_None;
        if (wcsstr(wszEnvVar, L"Errors") != NULL)
        {
            loggingType = (LoggingFlags)(loggingType | LoggingFlags_Errors);
        }
        if (wcsstr(wszEnvVar, L"Messages") != NULL)
        {
            loggingType = (LoggingFlags)(loggingType | LoggingFlags_Trace);
        }
        if (wcsstr(wszEnvVar, L"Dumps") != NULL)
        {
            loggingType = (LoggingFlags)(loggingType | LoggingFlags_InstrumentationResults);
        }
        if (wcsstr(wszEnvVar, L"All") != NULL)
        {
            loggingType = (LoggingFlags)(LoggingFlags_Errors | LoggingFlags_Trace | LoggingFlags_InstrumentationResults);
        }

        if (loggingType != LoggingFlags_None)
        {
            //Set to true to enable file level logging
            spLogger->SetLoggingFlags(loggingType);
        }
    }
}

_Check_return_ HRESULT CExtensionsHost::InternalInitialize(
    _In_ const IProfilerManagerSptr& spProfilerManager)
{
    IProfilerManagerLoggingSptr spLogger = NULL;
    IfFailRet(spProfilerManager->GetLoggingInstance(&spLogger));
    this->SetLogger(spLogger);
    SetLoggingFlags(spLogger);

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
        TraceError(_T("No Instrumentation method configs found. Cancelling profiler host initialization."));
        return S_FALSE;
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
        TraceMsg(L"Raw profiler hook module is not specified, skip loading, path:");
    }

    return S_OK;
}