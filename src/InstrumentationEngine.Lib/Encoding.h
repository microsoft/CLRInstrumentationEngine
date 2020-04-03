// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#pragma warning(push)
#pragma warning(disable:4996) // ignore deprecated declaration.
#include <atlbase.h>
#pragma warning(pop)

class CEncoding
{
public:
    // Converts a utf-16 string to a utf-8 string allocating the resulting string
    // with new. Callers must call delete[] on the utf8 string.
    static HRESULT ConvertUtf16ToUtf8(_In_ const WCHAR* utf16String, _Inout_ ATL::CAutoVectorPtr<char>& pUtf8String);

    // Converts a utf-8 string to a utf-16 string allocating the resulting string
    // with new. Callers must call delete[] on the utf16 string.
    static HRESULT ConvertUtf8ToUtf16(_In_ const char* utf8String, _Inout_ ATL::CAutoVectorPtr<WCHAR>& pUtf16String);

    static HRESULT ConvertUtf8ToUtf16Bstr(_In_ const char* utf8String, _Out_ BSTR* pUtf16String);
};