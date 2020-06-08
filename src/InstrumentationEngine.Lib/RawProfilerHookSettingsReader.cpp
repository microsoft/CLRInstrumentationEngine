// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "RawProfilerHookSettingsReader.h"

namespace MicrosoftInstrumentationEngine
{
    LPCWSTR cszRawProfilerHookVarName = L"MicrosoftInstrumentationEngine_RawProfilerHook";
    LPCWSTR cszRawProfilerHookPathVarNameNoBitness = L"MicrosoftInstrumentationEngine_RawProfilerHookPath";

    LPCWSTR cszCoreRawProfilerHookVarName = L"MicrosoftInstrumentationEngine_CoreRawProfilerHook";
    LPCWSTR cszCoreRawProfilerHookPathVarNameNoBitness = L"MicrosoftInstrumentationEngine_CoreRawProfilerHookPath";
#ifdef X86
    LPCWSTR cszRawProfilerHookPathVarName = L"MicrosoftInstrumentationEngine_RawProfilerHookPath_32";
    LPCWSTR cszCoreRawProfilerHookPathVarName = L"MicrosoftInstrumentationEngine_CoreRawProfilerHookPath_32";
#else
    LPCWSTR cszRawProfilerHookPathVarName = L"MicrosoftInstrumentationEngine_RawProfilerHookPath_64";
    LPCWSTR cszCoreRawProfilerHookPathVarName = L"MicrosoftInstrumentationEngine_CoreRawProfilerHookPath_64";
#endif

    CRawProfilerHookSettingsReader::CRawProfilerHookSettingsReader() noexcept
    {
    }

    HRESULT CRawProfilerHookSettingsReader::ReadSettings(
        _In_ COR_PRF_RUNTIME_TYPE runtimeType,
        _Out_ GUID& clsidRawProfilerHookComponent,
        _Inout_ std::wstring& strRawProfilerHookModulePath)
    {
        HRESULT hr = S_OK;
        clsidRawProfilerHookComponent = GUID_NULL;

        std::wstring strRawProfilerHookClsid;

        // If CoreCLR RPH variable is not set, fallback to the desktop one if it exists.
        // We only want to use the CoreCLR RPH path variable if the CoreCLR classId was set.
        bool fUseCoreClrPath; 
        if (runtimeType == COR_PRF_RUNTIME_TYPE::COR_PRF_CORE_CLR)
        {
            fUseCoreClrPath = true;
            CLogging::LogMessage(_T("Examining coreCLR environment variable: %s"), cszCoreRawProfilerHookVarName);
            IfFailRet(GetEnvironmentVariableWrapper(cszCoreRawProfilerHookVarName, strRawProfilerHookClsid));
        }

        if (runtimeType == COR_PRF_RUNTIME_TYPE::COR_PRF_DESKTOP_CLR ||
            S_FALSE == hr)
        {
            fUseCoreClrPath = false; // either we're in desktop CLR or coreCLR RPH variable was not found.
            CLogging::LogMessage(_T("Examining desktop CLR environment variable: %s"), cszRawProfilerHookVarName);
            IfFailRet(GetEnvironmentVariableWrapper(cszRawProfilerHookVarName, strRawProfilerHookClsid));

            if (S_FALSE == hr)
            {
                CLogging::LogMessage(_T("RawProfilerHookComponent is not set"));
                return hr;
            }
        }

        IfFalseRet(strRawProfilerHookClsid.size() > 0, HRESULT_FROM_WIN32(ERROR_ENVVAR_NOT_FOUND));
        CLogging::LogMessage(_T("RawProfilerHookComponent specified, CLSID = %s"), strRawProfilerHookClsid);
        IfFailRet(CLSIDFromString(strRawProfilerHookClsid.c_str(), &clsidRawProfilerHookComponent));

        if (fUseCoreClrPath)
        {
            CLogging::LogMessage(_T("Examining coreCLR path environment variable: %s"), cszCoreRawProfilerHookPathVarName);
            IfFailRet(GetEnvironmentVariableWrapper(cszCoreRawProfilerHookPathVarName, strRawProfilerHookModulePath));

            // This is for backwards compatibility; fall back to the no-bitness environment variable
            //    MicrosoftInstrumentationEngine_CoreRawProfilerHookPath
            if (S_FALSE == hr)
            {
                CLogging::LogMessage(_T("Examining coreCLR path environment variable (no bitness): %s"), cszCoreRawProfilerHookPathVarNameNoBitness);
                IfFailRet(GetEnvironmentVariableWrapper(cszCoreRawProfilerHookPathVarNameNoBitness, strRawProfilerHookModulePath));
            }
        }

        if (!fUseCoreClrPath ||
            S_FALSE == hr)
        {
            CLogging::LogMessage(_T("Examining desktop CLR path environment variable: %s"), cszRawProfilerHookPathVarName);
            IfFailRet(GetEnvironmentVariableWrapper(cszRawProfilerHookPathVarName, strRawProfilerHookModulePath));

            // This is for backwards compatibility; fall back to the no-bitness environment variable
            //    MicrosoftInstrumentationEngine_RawProfilerHookPath
            if (S_FALSE == hr)
            {
                CLogging::LogMessage(_T("Examining desktop CLR path environment variable (no bitness): %s"), cszRawProfilerHookPathVarNameNoBitness);

                IfFailRet(GetEnvironmentVariableWrapper(cszRawProfilerHookPathVarNameNoBitness, strRawProfilerHookModulePath));
            }
        }

        CLogging::LogMessage(_T("RawProfilerHook ModulePath specified, path = %s"), strRawProfilerHookModulePath);

        // if cszRawProfilerHookPathVarName and cszRawProfilerHookPathVarNameNoBitness are not set then we consider it as error;
        // there's no path to the component module to load
        IfFalseRet(S_FALSE != hr, HRESULT_FROM_WIN32(ERROR_ENVVAR_NOT_FOUND));
        IfFalseRet(strRawProfilerHookModulePath.size() > 0, HRESULT_FROM_WIN32(ERROR_ENVVAR_NOT_FOUND));

        return S_OK;
    }

    HRESULT CRawProfilerHookSettingsReader::GetEnvironmentVariableWrapper(
        _In_ const std::wstring& strVarName,
        _Out_ std::wstring& strVariableValue)
    {
        const int MaxVariableSize = 1024;
        std::wstring strVariableValueTemp(1024, L'\n');

        auto dwCount = ::GetEnvironmentVariable(
            strVarName.c_str(),
            &strVariableValueTemp[0],
            MaxVariableSize);

        strVariableValueTemp.resize(dwCount);
        strVariableValue.swap(strVariableValueTemp);

        if (0 == dwCount)
        {
            auto dwLastError = ::GetLastError();
            if (ERROR_ENVVAR_NOT_FOUND != dwLastError)
            {
                return HRESULT_FROM_WIN32(dwLastError);
            }

            return S_FALSE;
        }

        return S_OK;
    }
}