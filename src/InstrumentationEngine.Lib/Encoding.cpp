// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "Encoding.h"
#include "../Common.Lib/StringUtils.h"

using namespace ATL;

HRESULT CEncoding::ConvertUtf16ToUtf8(_In_ const WCHAR* utf16String, _Inout_ CAutoVectorPtr<char>& pUtf8String)
{
    HRESULT hr = S_OK;

    CW2A cw2a(utf16String, CP_UTF8);
    size_t utf8BufLen = 0;
    IfFailRet(StringUtils::StringLen(cw2a, utf8BufLen));
    ++utf8BufLen;

    pUtf8String.Free();
    pUtf8String.Allocate(utf8BufLen);

    if (pUtf8String == nullptr)
    {
        return E_OUTOFMEMORY;
    }
    IfFailRetErrno(memcpy_s(pUtf8String, utf8BufLen * sizeof(char), static_cast<LPCSTR>(cw2a), utf8BufLen * sizeof(char)));

    return S_OK;
}

HRESULT CEncoding::ConvertUtf8ToUtf16(_In_ const char* utf8String, _Inout_ CAutoVectorPtr<WCHAR>& pUtf16String)
{
    HRESULT hr = S_OK;

    CA2W ca2w(utf8String, CP_UTF8);
    size_t utf16BufLen = 0;
    IfFailRet(StringUtils::WStringLen(ca2w, utf16BufLen));

    ++utf16BufLen;
    pUtf16String.Free();
    IfFalseRet(pUtf16String.Allocate(utf16BufLen), E_FAIL);

    if (pUtf16String == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    IfFailRetErrno(memcpy_s(pUtf16String, utf16BufLen * sizeof(WCHAR), static_cast<LPCWSTR>(ca2w), utf16BufLen * sizeof(WCHAR)));

    return S_OK;
}

HRESULT CEncoding::ConvertUtf8ToUtf16Bstr(_In_ const char* utf8String, _Out_ BSTR* pUtf16String)
{
    HRESULT hr = S_OK;

    CAutoVectorPtr<WCHAR> newStringBuffer;
    IfFailRet(ConvertUtf8ToUtf16(utf8String, newStringBuffer));

    CComBSTR bstrUtf16String = newStringBuffer.m_p; // Copy string.
    *pUtf16String = bstrUtf16String.Detach(); // Detach so that string doesn't get cleaned up.

    return S_OK;
}
