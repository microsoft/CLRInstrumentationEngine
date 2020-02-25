// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "RawProfilerHookSettingsReader.h"

namespace MicrosoftInstrumentationEngine
{
    LPCWSTR cszRawProfilerHookVariableName = L"MicrosoftInstrumentationEngine_RawProfilerHook";
    LPCWSTR cszRawProfilerHookPathVariableNameNoBitness = L"MicrosoftInstrumentationEngine_RawProfilerHookPath";
#ifdef X86
    LPCWSTR cszRawProfilerHookPathVariableName = L"MicrosoftInstrumentationEngine_RawProfilerHookPath_32";
#else
    LPCWSTR cszRawProfilerHookPathVariableName = L"MicrosoftInstrumentationEngine_RawProfilerHookPath_64";
#endif

    CRawProfilerHookSettingsReader::CRawProfilerHookSettingsReader() noexcept
    {
    }

    HRESULT CRawProfilerHookSettingsReader::ReadSettings(
        _Out_ GUID& clsidRawProfilerHookComponent,
        _Inout_ std::wstring& strRawProfilerHookModulePath)
    {
        HRESULT hr = S_OK;
        clsidRawProfilerHookComponent = GUID_NULL;

        std::wstring strRawProfilerHookClsid;

        CLogging::LogMessage(_T("Examining environment variable: %s"), cszRawProfilerHookVariableName);

        IfFailRet(GetEnvironmentVariable(cszRawProfilerHookVariableName, strRawProfilerHookClsid));
        if (S_FALSE == hr)
        {
            CLogging::LogMessage(_T("RawProfilerHookComponent is not set"));
            return hr;
        }

        IfFalseRet(strRawProfilerHookClsid.size() > 0, HRESULT_FROM_WIN32(ERROR_ENVVAR_NOT_FOUND));

        CLogging::LogMessage(_T("RawProfilerHookComponent specified, CLSID = %s"), strRawProfilerHookClsid);

        IfFailRet(CLSIDFromString(strRawProfilerHookClsid.c_str(), &clsidRawProfilerHookComponent));

        CLogging::LogMessage(_T("Examining environment variable: %s"), cszRawProfilerHookPathVariableName);

        IfFailRet(GetEnvironmentVariable(cszRawProfilerHookPathVariableName, strRawProfilerHookModulePath));

        // This is for backwards compatibility; fall back to the no-bitness environment variable
        //    MicrosoftInstrumentationEngine_RawProfilerHookPath
        if (S_FALSE == hr)
        {
            CLogging::LogMessage(_T("Examining environment variable: %s"), cszRawProfilerHookPathVariableNameNoBitness);

            IfFailRet(GetEnvironmentVariable(cszRawProfilerHookPathVariableNameNoBitness, strRawProfilerHookModulePath));
        }

        CLogging::LogMessage(_T("RawProfilerHookModulePath specified, path = %s"), strRawProfilerHookModulePath);

        // if cszRawProfilerHookPathVariableName and cszRawProfilerHookPathVariableNameNoBitness are not set then we consider it as error;
        // there's no path to the component module to load
        IfFalseRet(S_FALSE != hr, HRESULT_FROM_WIN32(ERROR_ENVVAR_NOT_FOUND));
        IfFalseRet(strRawProfilerHookModulePath.size() > 0, HRESULT_FROM_WIN32(ERROR_ENVVAR_NOT_FOUND));

        return S_OK;
    }

    HRESULT CRawProfilerHookSettingsReader::GetEnvironmentVariable(
        _In_ const std::wstring& strVariableName,
        _Out_ std::wstring& strVariableValue)
    {
        const int MaxVariableSize = 1024;
        std::wstring strVariableValueTemp(1024, L'\n');

        auto dwCount = ::GetEnvironmentVariable(
            strVariableName.c_str(),
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