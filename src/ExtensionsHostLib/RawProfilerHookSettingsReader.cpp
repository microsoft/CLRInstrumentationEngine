// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "RawProfilerHookSettingsReader.h"

#include "ExtensionsCommon/Environment.h"

using Agent::Diagnostics::Param;

namespace Agent
{
namespace Host
{
    LPCWSTR cszRawProfilerHookVariableNameNoBitness = L"MicrosoftInstrumentationEngine_RawProfilerHook";
    LPCWSTR cszRawProfilerHookPathVariableNameNoBitness = L"MicrosoftInstrumentationEngine_RawProfilerHookPath";
#ifdef X86
    LPCWSTR cszRawProfilerHookVariableName = L"MicrosoftInstrumentationEngine_RawProfilerHook_32";
    LPCWSTR cszRawProfilerHookPathVariableName = L"MicrosoftInstrumentationEngine_RawProfilerHookPath_32";
#else
    LPCWSTR cszRawProfilerHookVariableName = L"MicrosoftInstrumentationEngine_RawProfilerHook_64";
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

        Settings::CEnvironment env;
        std::wstring strRawProfilerHookClsid;

        TraceMsg(
            L"Examining environment variable",
            Param(L"name", cszRawProfilerHookVariableName));

        IfFailRetHresult(env.GetEnvironmentVariable(cszRawProfilerHookVariableName, strRawProfilerHookClsid), hr);
        // If cszRawProfilerHookVariableName variable is not set
        // then check cszRawProfilerHookVariableNameNoBitness.
        if (S_FALSE == hr)
        {
            TraceMsg(
                L"Examining environment variable",
                Param(L"name", cszRawProfilerHookVariableNameNoBitness));

            IfFailRetHresult(env.GetEnvironmentVariable(cszRawProfilerHookVariableNameNoBitness, strRawProfilerHookClsid), hr);
            if (S_FALSE == hr)
            {
                TraceMsg("RawProfilerHookComponent is not set");
                return hr;
            }
        }

        IfFalseRet(strRawProfilerHookClsid.size() > 0, HRESULT_FROM_WIN32(ERROR_ENVVAR_NOT_FOUND));

        TraceMsg(
            L"RawProfilerHookComponent specified",
            Param(L"CLSID", strRawProfilerHookClsid));

        IfFailRet(CLSIDFromString(strRawProfilerHookClsid.c_str(), &clsidRawProfilerHookComponent));

        TraceMsg(
            L"Examining environment variable",
            Param(L"name", cszRawProfilerHookPathVariableName));

        IfFailRetHresult(
            env.GetEnvironmentVariable(
                cszRawProfilerHookPathVariableName,
                strRawProfilerHookModulePath), hr);

        if (S_FALSE == hr)
        {
            TraceMsg(
                L"Examining environment variable",
                Param(L"name", cszRawProfilerHookPathVariableNameNoBitness));

            IfFailRetHresult(
                env.GetEnvironmentVariable(
                    cszRawProfilerHookPathVariableNameNoBitness,
                    strRawProfilerHookModulePath), hr);
        }

        TraceMsg(
            L"RawProfilerHookModulePath specified",
            Param(L"path", strRawProfilerHookModulePath));

        // if cszRawProfilerHookPathVariableName and cszRawProfilerHookPathVariableNameNoBitness are not set we consider it as error,
        // there's no path to the component module to load
        IfTrueRet(S_FALSE == hr, HRESULT_FROM_WIN32(ERROR_ENVVAR_NOT_FOUND));
        IfFalseRet(strRawProfilerHookModulePath.size() > 0, HRESULT_FROM_WIN32(ERROR_ENVVAR_NOT_FOUND));

        return S_OK;
    }
}
}