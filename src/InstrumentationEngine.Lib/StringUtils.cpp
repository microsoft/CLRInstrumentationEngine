// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "StringUtils.h"

#ifndef E_BOUNDS
// Apparantly, Linux builds don't have E_BOUNDS defined.
#define E_BOUNDS 0x8000000BL
#endif

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