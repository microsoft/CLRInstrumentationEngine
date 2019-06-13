// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "../ExtensionsCommon/ModuleHandle.h"
#include "../ExtensionsCommon/ProfilerHostServices.h"

namespace Agent
{
namespace Host
{
    class CRawProfilerHookLoader final : public CProfilerHostServices
    {
        CRawProfilerHookLoader(const CRawProfilerHookLoader&) = delete;
        CRawProfilerHookLoader& operator=(const CRawProfilerHookLoader&) = delete;
    public:
        CRawProfilerHookLoader() noexcept;

        _Check_return_ HRESULT LoadRawProfilerHookComponentFrom(
            _In_ const std::wstring& strModulePath,
            _In_ const GUID& clsidRawProfilerHook,
            _Out_ IUnknownSptr& spRawProfilerHook,
            _Out_ Io::CModuleHandle& hRawProfilerHookModule);
    }; // CRawProfilerHookLoader
} // Host
} // Agent