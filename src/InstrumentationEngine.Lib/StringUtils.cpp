// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "StringUtils.h"

// Somewhat arbitrary.
const size_t StringUtils::MAX_STRING_LEN = 10000;

HRESULT StringUtils::WStringLen(_In_ const WCHAR* wszString, _Out_ size_t& len)
{
    len = wcsnlen_s(wszString, MAX_STRING_LEN);

    if (len >= MAX_STRING_LEN)
    {
        return E_BOUNDS;
    }

    return S_OK;
}

HRESULT StringUtils::StringLen(_In_ const char* szString, _Out_ size_t& len)
{
    len = strnlen(szString, MAX_STRING_LEN);

    if (len >= MAX_STRING_LEN)
    {
        return E_BOUNDS;
    }

    return S_OK;
}

size_t StringUtils::WStringLen(_In_ const WCHAR* wszString)
{
    return wcsnlen_s(wszString, MAX_STRING_LEN);
}

size_t StringUtils::StringLen(_In_ const char* szString)
{
    return strnlen(szString, MAX_STRING_LEN);
}

HRESULT StringUtils::SafePathAppend(_Inout_updates_z_(cBounds) LPWSTR pwszPath, _In_ LPCWSTR pwszMore, _In_ size_t cBounds)
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