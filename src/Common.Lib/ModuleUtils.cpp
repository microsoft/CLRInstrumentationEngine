// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PathUtils.h"
#include "ModuleUtils.h"

namespace CommonLib
{

    HMODULE ModuleUtils::s_hModuleMarker = nullptr;

    /// <summary>
    /// Gets handle of the host module
    /// </summary>
    HINSTANCE ModuleUtils::GetHostModule()
    {
        if (nullptr == s_hModuleMarker)
        {
            ::GetModuleHandleEx(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCWSTR>(&s_hModuleMarker),
                &s_hModuleMarker);
        }

        return s_hModuleMarker;
    }

    std::wstring ModuleUtils::GetModuleDirectory(HINSTANCE hInstance)
    {
        std::wstring result(L"");
        wchar_t szRelativePath[_MAX_PATH + 1] = { 0 };

        if (0 != ::GetModuleFileName(hInstance, szRelativePath, _countof(szRelativePath)))
        {
            wchar_t szAbsolutePath[_MAX_PATH + 1] = { 0 };

            LPWSTR* lppPart = nullptr;
            ::GetFullPathName(szRelativePath, _countof(szAbsolutePath), szAbsolutePath, lppPart);

            result = PathUtils::GetDirFromPath(szAbsolutePath);
        }

        return result;
    }
}