// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace MicrosoftInstrumentationEngine
{
    class CRawProfilerHookSettingsReader final
    {
        CRawProfilerHookSettingsReader(const CRawProfilerHookSettingsReader&) = delete;
        CRawProfilerHookSettingsReader& operator=(const CRawProfilerHookSettingsReader&) = delete;
    public:
        CRawProfilerHookSettingsReader() noexcept;

        HRESULT ReadSettings(
            _Out_ GUID& clsidRawProfilerHookComponent,
            _Inout_ std::wstring& strRawProfilerHookModulePath);
    private:
        HRESULT GetEnvironmentVariable(
            _In_ const std::wstring& strVariableName,
            _Out_ std::wstring& strVariableValue);
    }; // CRawProfilerHookLoader
}