// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "systemstring.h"
#include <stdint.h>
#include <string.h>
#include <memory>

using namespace std;

namespace CommonLib
{
#ifdef PLATFORM_UNIX

    namespace
    {
        static const SIZE_T MAX_STRING_LEN = 10000;
        static_assert(sizeof(WCHAR) == 2, "SystemString requires 16-bit WCHAR values");

        bool DecodeUtf8CodePoint(
            const CHAR* input,
            size_t inputLength,
            size_t& index,
            uint32_t& codePoint,
            size_t& continuationCount)
        {
            const unsigned char lead = static_cast<unsigned char>(input[index++]);
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
                return false;
            }

            if (continuationCount > inputLength - index)
            {
                return false;
            }

            for (size_t i = 0; i < continuationCount; ++i)
            {
                const unsigned char continuation = static_cast<unsigned char>(input[index++]);
                if ((continuation & 0xC0) != 0x80)
                {
                    return false;
                }
                codePoint = (codePoint << 6) | (continuation & 0x3F);
            }

            return true;
        }

        bool IsValidUtf8CodePoint(uint32_t codePoint, size_t continuationCount)
        {
            const uint32_t minimumCodePoint = continuationCount == 1 ? 0x80
                : continuationCount == 2 ? 0x800
                : continuationCount == 3 ? 0x10000
                : 0;
            return codePoint >= minimumCodePoint &&
                codePoint <= 0x10FFFF &&
                (codePoint < 0xD800 || codePoint > 0xDFFF);
        }

        void AppendUtf16CodePoint(uint32_t codePoint, tstring& output)
        {
            if (codePoint <= 0xFFFF)
            {
                output.push_back(static_cast<WCHAR>(codePoint));
                return;
            }

            codePoint -= 0x10000;
            output.push_back(static_cast<WCHAR>(0xD800 + (codePoint >> 10)));
            output.push_back(static_cast<WCHAR>(0xDC00 + (codePoint & 0x3FF)));
        }

        bool DecodeUtf16CodePoint(
            const WCHAR* input,
            size_t inputLength,
            size_t& index,
            uint32_t& codePoint)
        {
            codePoint = static_cast<uint16_t>(input[index++]);
            if (codePoint >= 0xD800 && codePoint <= 0xDBFF)
            {
                if (index >= inputLength)
                {
                    return false;
                }

                const uint32_t lowSurrogate = static_cast<uint16_t>(input[index++]);
                if (lowSurrogate < 0xDC00 || lowSurrogate > 0xDFFF)
                {
                    return false;
                }

                codePoint = 0x10000 + ((codePoint - 0xD800) << 10) + (lowSurrogate - 0xDC00);
            }
            else if (codePoint >= 0xDC00 && codePoint <= 0xDFFF)
            {
                return false;
            }

            return true;
        }

        void AppendUtf8CodePoint(uint32_t codePoint, string& output)
        {
            if (codePoint <= 0x7F)
            {
                output.push_back(static_cast<char>(codePoint));
            }
            else if (codePoint <= 0x7FF)
            {
                output.push_back(static_cast<char>(0xC0 | (codePoint >> 6)));
                output.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
            }
            else if (codePoint <= 0xFFFF)
            {
                output.push_back(static_cast<char>(0xE0 | (codePoint >> 12)));
                output.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
                output.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
            }
            else
            {
                output.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
                output.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
                output.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
                output.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
            }
        }
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
        for (size_t index = 0; index < inputLength;)
        {
            uint32_t codePoint;
            size_t continuationCount;
            if (!DecodeUtf8CodePoint(
                    lpzStr,
                    inputLength,
                    index,
                    codePoint,
                    continuationCount) ||
                !IsValidUtf8CodePoint(codePoint, continuationCount))
            {
                result.clear();
                return E_INVALIDARG;
            }

            AppendUtf16CodePoint(codePoint, converted);
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
        for (size_t index = 0; index < inputLength;)
        {
            uint32_t codePoint;
            if (!DecodeUtf16CodePoint(lpzwStr, inputLength, index, codePoint))
            {
                result.clear();
                return E_INVALIDARG;
            }

            AppendUtf8CodePoint(codePoint, converted);
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