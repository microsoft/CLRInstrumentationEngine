// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "RawProfilerHookSettingsReader.h"

#include "ExtensionsCommon/Environment.h"

using Agent::Diagnostics::Param;

namespace Agent
{
namespace Host
{
    LPCWSTR cszRawProfilerHookVariableName = L"MicrosoftInstrumentationEngine_RawProfilerHook";
    LPCWSTR cszRawProfilerHookPathVariableName = L"MicrosoftInstrumentationEngine_RawProfilerHookPath";

    CRawProfilerHookSettingsReader::CRawProfilerHookSettingsReader() noexcept
    {
    }

    HRESULT CRawProfilerHookSettingsReader::ReadSettings(
        _Out_ GUID& clsidRawProfilerHookComponent,
        _Inout_ std::wstring& strRawProfilerHookModulePath)
    {
        TraceMsg(
            L"Examining environment variable",
            Param(L"name", cszRawProfilerHookVariableName));

        HRESULT hr = S_OK;
        clsidRawProfilerHookComponent = GUID_NULL;

        Settings::CEnvironment env;
        std::wstring strRawProfilerHookClsid;
        IfFailRetHresult(env.GetEnvironmentVariable(cszRawProfilerHookVariableName, strRawProfilerHookClsid), hr);

        // cszRawProfilerHookVariableName variable is not set, assuming that it's not configured intentionally
        // so we don't need to load any raw hooks
        if (S_FALSE == hr)
        {
            TraceMsg("RawProfilerHookComponent is not set");

            return hr;
        }

        IfFalseRet(strRawProfilerHookClsid.size() > 0, HRESULT_FROM_WIN32(ERROR_ENVVAR_NOT_FOUND));

        TraceMsg(
            L"RawProfilerHookComponent specified",
            Param(L"CLSID", strRawProfilerHookClsid));

        IfFailRet(CLSIDFromString(strRawProfilerHookClsid.c_str(), &clsidRawProfilerHookComponent));

        TraceMsg(
            L"Examining environment variable, name:",
            Param(L"name", cszRawProfilerHookPathVariableName));

        IfFailRetHresult(
            env.GetEnvironmentVariable(
                cszRawProfilerHookPathVariableName,
                strRawProfilerHookModulePath), hr);
        TraceMsg(
            L"RawProfilerHookModulePath specified",
            Param(L"path", strRawProfilerHookModulePath));

        // if cszRawProfilerHookPathVariableName is not set we consider it as error,
        // there's no path to the component module to load
        IfTrueRet(S_FALSE == hr, HRESULT_FROM_WIN32(ERROR_ENVVAR_NOT_FOUND));
        IfFalseRet(strRawProfilerHookModulePath.size() > 0, HRESULT_FROM_WIN32(ERROR_ENVVAR_NOT_FOUND));

        return S_OK;
    }
}
}