// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Agent
{
namespace Io
{
    class CModuleHandle final
    {
        HMODULE     m_hModule;
        CModuleHandle(const CModuleHandle&) = delete;
        CModuleHandle& operator= (const CModuleHandle&) = delete;
    public:
        CModuleHandle(const HMODULE hModule = nullptr) noexcept;
        ~CModuleHandle();

        const HMODULE GetHandle() const;
        operator const HMODULE() const;

        void Reset(const HMODULE hModule = nullptr);
    }; // CModuleHandle
}
}