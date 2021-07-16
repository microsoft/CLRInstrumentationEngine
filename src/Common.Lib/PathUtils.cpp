// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PathUtils.h"
#include "StringUtils.h"


namespace CommonLib
{
    bool PathUtils::Exists(const std::wstring& strPath)
    {
        return ::GetFileAttributes(strPath.c_str()) != INVALID_FILE_ATTRIBUTES;
    }

    std::wstring PathUtils::GetDirFromPath(LPCWSTR path)
    {
        wchar_t drive[_MAX_DRIVE + 1] = { 0 };
        wchar_t dir[_MAX_DIR + 1] = { 0 };

        // parse app path and get app name
        _wsplitpath_s(path, drive, _countof(drive), dir, _countof(dir), 0, 0, 0, 0);

        std::wstring dirName(drive);
        dirName += dir;

        return dirName;
    }

    std::wstring PathUtils::Combine(const std::wstring& strPath1, const std::wstring& strPath2)
    {
        std::wostringstream strFormat;

        strFormat << strPath1;
        if (strPath1.back() != L'\\') { strFormat << L'\\'; }
        strFormat << strPath2;

        return strFormat.str();
    }

    HRESULT PathUtils::SafePathAppend(LPWSTR pwszPath, LPCWSTR pwszMore, size_t cBounds)
    {
        // If PathAppend fails, the destination buffer will be cleared. Check bounds before appending.
        if (StringUtils::WStringLen(pwszPath) + StringUtils::WStringLen(pwszMore) >= cBounds)
        {
            return E_BOUNDS;
        }

        if (!PathAppend(pwszPath, pwszMore))
        {
            return E_FAIL;
        }

        return S_OK;
    }
}