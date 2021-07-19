// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>

class StringUtils
{
public:

    // The maximum string length. Set to 10000;
    static const size_t MAX_STRING_LEN;

    // Gets the length of the given string using the the standard wcsnlen_s.
    // Returns E_BOUNDS if the length of wszString is greater than MAX_STRING_LEN.
    // For use with untrusted data which may not have a null terminator.
    static HRESULT WStringLen(_In_ const WCHAR* wszString, _Out_ size_t& len);

    // Gets the length of the given string using the standard strnlen.
    // Returns E_BOUNDS if the length of wszString is greater than MAX_STRING_LEN.
    // For use with untrusted data which may not have a null terminator.
    static HRESULT StringLen(_In_ const char* szString, _Out_ size_t& len);

    // Gets the length of the given string using the standard wcsnlen_s.
    // Yields the same result as calling wcsnlen_s(wszString, MAX_STRING_LEN);
    // For use with untrusted data which may not have a null terminator.
    static size_t WStringLen(_In_ const WCHAR* wszString);

    // Gets the length of the given string using the standard strnlen.
    // Yields the same result as calling strnlen(szString, MAX_STRING_LEN);
    // For use with untrusted data which may not have a null terminator.
    static size_t StringLen(_In_ const char* szString);

    // Modifies pwszPath by appending pwszMore.
    // Checks cBounds before appending to prevent buffer overflows.
    // 
    // This function wraps PathAppend since supporting Win7 means PathCchAppend is not available.
    static HRESULT SafePathAppend(_Inout_updates_z_(cBounds) LPWSTR pwszPath, _In_ LPCWSTR pwszMore, _In_ size_t cBounds);
};