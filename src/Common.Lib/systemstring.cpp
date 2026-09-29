// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "systemstring.h"
#include <stdint.h>
#include <memory>

using namespace std;

namespace CommonLib
{
#ifdef PLATFORM_UNIX

    static const SIZE_T MAX_STRING_LEN = 10000;
    static_assert(sizeof(WCHAR) == 2, "SystemString requires 16-bit WCHAR values");

    HRESULT SystemString::Convert(_In_z_ const CHAR* lpzStr, _Inout_ tstring& result)
    {
        if (lpzStr == nullptr)
        {
            result = _T("");
            return E_INVALIDARG;
        }

        size_t inputLength = 0;
        while (inputLength < MAX_STRING_LEN && lpzStr[inputLength] != '\0')
        {
            ++inputLength;
        }
        if (inputLength + 1 >= MAX_STRING_LEN)
        {
            return E_BOUNDS;
        }

        tstring converted;
        converted.reserve(inputLength);
        for (size_t i = 0; i < inputLength;)
        {
            const unsigned char lead = static_cast<unsigned char>(lpzStr[i++]);
            uint32_t codePoint;
            size_t continuationCount;
            if (lead <= 0x7F)
            {
                codePoint = lead;
                continuationCount = 0;
            }
            else if ((lead & 0xE0) == 0xC0)
            {
                codePoint = lead & 0x1F;
                continuationCount = 1;
            }
            else if ((lead & 0xF0) == 0xE0)
            {
                codePoint = lead & 0x0F;
                continuationCount = 2;
            }
            else if ((lead & 0xF8) == 0xF0)
            {
                codePoint = lead & 0x07;
                continuationCount = 3;
            }
            else
            {
                result.clear();
                return E_INVALIDARG;
            }

            if (i + continuationCount > inputLength)
            {
                result.clear();
                return E_INVALIDARG;
            }

            for (size_t j = 0; j < continuationCount; ++j)
            {
                const unsigned char continuation = static_cast<unsigned char>(lpzStr[i++]);
                if ((continuation & 0xC0) != 0x80)
                {
                    result.clear();
                    return E_INVALIDARG;
                }
                codePoint = (codePoint << 6) | (continuation & 0x3F);
            }

            const uint32_t minimumCodePoint = continuationCount == 1 ? 0x80
                : continuationCount == 2 ? 0x800
                : continuationCount == 3 ? 0x10000
                : 0;
            if (codePoint < minimumCodePoint || codePoint > 0x10FFFF ||
                (codePoint >= 0xD800 && codePoint <= 0xDFFF))
            {
                result.clear();
                return E_INVALIDARG;
            }

            if (codePoint <= 0xFFFF)
            {
                converted.push_back(static_cast<WCHAR>(codePoint));
            }
            else
            {
                codePoint -= 0x10000;
                converted.push_back(static_cast<WCHAR>(0xD800 + (codePoint >> 10)));
                converted.push_back(static_cast<WCHAR>(0xDC00 + (codePoint & 0x3FF)));
            }
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
        for (size_t i = 0; i < inputLength; ++i)
        {
            uint32_t codePoint = static_cast<uint16_t>(lpzwStr[i]);
            if (codePoint >= 0xD800 && codePoint <= 0xDBFF)
            {
                if (++i >= inputLength)
                {
                    result.clear();
                    return E_INVALIDARG;
                }

                const uint32_t lowSurrogate = static_cast<uint16_t>(lpzwStr[i]);
                if (lowSurrogate < 0xDC00 || lowSurrogate > 0xDFFF)
                {
                    result.clear();
                    return E_INVALIDARG;
                }
                codePoint = 0x10000 + ((codePoint - 0xD800) << 10) + (lowSurrogate - 0xDC00);
            }
            else if (codePoint >= 0xDC00 && codePoint <= 0xDFFF)
            {
                result.clear();
                return E_INVALIDARG;
            }

            if (codePoint <= 0x7F)
            {
                converted.push_back(static_cast<char>(codePoint));
            }
            else if (codePoint <= 0x7FF)
            {
                converted.push_back(static_cast<char>(0xC0 | (codePoint >> 6)));
                converted.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
            }
            else if (codePoint <= 0xFFFF)
            {
                converted.push_back(static_cast<char>(0xE0 | (codePoint >> 12)));
                converted.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
                converted.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
            }
            else
            {
                converted.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
                converted.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
                converted.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
                converted.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
            }
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