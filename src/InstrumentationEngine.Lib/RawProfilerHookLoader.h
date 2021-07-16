// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "../Common.Lib/ModuleHandle.h"

namespace MicrosoftInstrumentationEngine
{
    class CRawProfilerHookLoader final
    {
        CRawProfilerHookLoader(const CRawProfilerHookLoader&) = delete;
        CRawProfilerHookLoader& operator=(const CRawProfilerHookLoader&) = delete;
    public:
        CRawProfilerHookLoader() noexcept = default;

        _Check_return_ HRESULT LoadRawProfilerHookComponentFrom(
            _In_ const std::wstring& strModulePath,
            _In_ const GUID& clsidRawProfilerHook,
            _Out_ ATL::CComPtr<IUnknown>& spRawProfilerHook,
            _Out_ CommonLib::CModuleHandle& hRawProfilerHookModule);
    }; // CRawProfilerHookLoader
}