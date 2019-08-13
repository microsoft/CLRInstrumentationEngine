// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Agent
{
namespace Host
{
    class CRawProfilerHookSettingsReader final : public CProfilerHostServices
    {
        CRawProfilerHookSettingsReader(const CRawProfilerHookSettingsReader&) = delete;
        CRawProfilerHookSettingsReader& operator=(const CRawProfilerHookSettingsReader&) = delete;
    public:
        CRawProfilerHookSettingsReader() noexcept;

        HRESULT ReadSettings(
            _Out_ GUID& clsidRawProfilerHookComponent,
            _Inout_ std::wstring& strRawProfilerHookModulePath);
    }; // CRawProfilerHookLoader
}
}