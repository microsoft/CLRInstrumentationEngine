// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CommonLib
{
    class ModuleUtils
    {
    private:
        static HINSTANCE s_hModuleMarker;

    public:
        static HINSTANCE GetHostModule();
        static std::wstring GetModuleDirectory(HINSTANCE hInstance);
    };
}