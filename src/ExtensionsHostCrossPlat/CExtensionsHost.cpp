// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// CExtensionsHost.cpp : Implementation of CExtensionHost

#include "stdafx.h"

#include "CExtensionsHost.h"

// static
void ExtensionsHostCrossPlat::CExtensionHost::SetLoggingFlags(_In_ IProfilerManagerLogging* pLogger)
{
    WCHAR wszEnvVar[MAX_PATH];
    if (GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_FileLog"), wszEnvVar, MAX_PATH) > 0)
    {
        LoggingFlags loggingType = LoggingFlags_None;
        if (PAL_wcsstr(wszEnvVar, _T("Errors")) != NULL)
        {
            loggingType = (LoggingFlags)(loggingType | LoggingFlags_Errors);
        }
        if (PAL_wcsstr(wszEnvVar, _T("Messages")) != NULL)
        {
            loggingType = (LoggingFlags)(loggingType | LoggingFlags_Trace);
        }
        if (PAL_wcsstr(wszEnvVar, _T("Dumps")) != NULL)
        {
            loggingType = (LoggingFlags)(loggingType | LoggingFlags_InstrumentationResults);
        }
        if (PAL_wcsstr(wszEnvVar, _T("All")) != NULL)
        {
            loggingType = (LoggingFlags)(LoggingFlags_Errors | LoggingFlags_Trace | LoggingFlags_InstrumentationResults);
        }

        if (loggingType != LoggingFlags_None)
        {
            //Set to true to enable file level logging
            pLogger->SetLoggingFlags(loggingType);
        }
    }
}

HRESULT ExtensionsHostCrossPlat::CExtensionHost::Initialize(
    _In_ IProfilerManager* pProfilerManager
)
{
    HRESULT hr = S_OK;

    CComPtr<IProfilerManagerLogging> pLogger;
    IfFailRet(pProfilerManager->GetLoggingInstance(&pLogger));
    SetLoggingFlags(pLogger);

#ifdef X86
    WCHAR wszHostPathVariableName[] = _T("MicrosoftInstrumentationEngine_HostPath_32");
#else
    WCHAR wszHostPathVariableName[] = _T("MicrosoftInstrumentationEngine_HostPath_64");
#endif

    WCHAR wszProfilerPath[MAX_PATH];
    if (!GetEnvironmentVariable(wszHostPathVariableName, wszProfilerPath, MAX_PATH))
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
