// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "systemstring.h"
#include <memory>

#ifdef PLATFORM_UNIX
#include <string.h>
#include <utf8.h>
#endif

using namespace std;

namespace CommonLib
{
#ifdef PLATFORM_UNIX

    namespace
    {
        static const SIZE_T MAX_STRING_LEN = 10000;
        static_assert(sizeof(WCHAR) == 2, "SystemString requires 16-bit WCHAR values");
    }

    HRESULT SystemString::Convert(_In_z_ const CHAR* lpzStr, _Inout_ tstring& result)
    {
        if (lpzStr == nullptr)
        {
            result = _T("");
            return E_INVALIDARG;
        }

        const size_t inputLength = strnlen(lpzStr, MAX_STRING_LEN);
        if (inputLength >= MAX_STRING_LEN - 1)
        {
            return E_BOUNDS;
        }

        tstring converted;
        converted.reserve(inputLength);
        try
        {
            utf8::utf8to16(lpzStr, lpzStr + inputLength, back_inserter(converted));
        }
        catch (const utf8::exception&)
        {
            result.clear();
            return E_INVALIDARG;
        }

        result.swap(converted);
        return S_OK;
    }

    HRESULT SystemString::Convert(_In_z_ const WCHAR* lpzwStr, _Inout_ string& result)
    {
        if (lpzwStr == nullptr)
        {
            result = "";
            return E_INVALIDARG;
        }

        size_t inputLength = 0;
        for (; inputLength < MAX_STRING_LEN; ++inputLength)
        {
            if (lpzwStr[inputLength] == static_cast<WCHAR>(0))
            {
                break;
            }
        }

        if (inputLength >= MAX_STRING_LEN)
        {
            result.clear();
            return E_BOUNDS;
        }

        string converted;
        converted.reserve(inputLength);
        try
        {
            utf8::utf16to8(lpzwStr, lpzwStr + inputLength, back_inserter(converted));
        }
        catch (const utf8::exception&)
        {
            result.clear();
            return E_INVALIDARG;
        }

        result.swap(converted);
        return S_OK;
    }
#else


    HRESULT SystemString::Convert(_In_z_ const WCHAR* lpzwStr, _Inout_ string& result)
    {

        // WideCharToMultiByte will null check lpzwStr, so we don't need to.
        int required = WideCharToMultiByte(CP_UTF8, 0, lpzwStr, /*null terminated*/-1, nullptr, 0, 0, 0);
        if (required > 0)
        {
            unique_ptr<char[]> buffer(new char[required]);
            int written = WideCharToMultiByte(CP_UTF8, 0, lpzwStr, /*null terminated*/-1, buffer.get(), required, 0, 0);
            if (written > 0)
            {
                result = buffer.get();
                return S_OK;
            }
        }

        DWORD errorCode = GetLastError();
        result = "";
        return HRESULT_FROM_WIN32(errorCode);
    }

    HRESULT SystemString::Convert(_In_z_ const CHAR* lpzStr, _Inout_ tstring& result)
    {
        // MultiByteToWideChar will null check lpzStr, so we don't need to.
        int required = MultiByteToWideChar(CP_UTF8, 0, lpzStr, /*null terminated*/ -1, nullptr, 0);
        if (required > 0)
        {
            unique_ptr<WCHAR[]> buffer(new WCHAR[required]);
            int written = MultiByteToWideChar(CP_UTF8, 0, lpzStr, /*null terminated*/ -1, buffer.get(), required);
            if (written > 0)
            {
                result = buffer.get();
                return S_OK;
            }
        }

        DWORD errorCode = GetLastError();
        result = L"";
        return HRESULT_FROM_WIN32(errorCode);
    }
#endif
}