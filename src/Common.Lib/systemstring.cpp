// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "systemstring.h"
#include <vector>
#include <memory>

#ifdef PLATFORM_UNIX
#include <errno.h>
#include <iconv.h>
#endif

using namespace std;

namespace CommonLib
{
#ifdef PLATFORM_UNIX

    static const SIZE_T MAX_STRING_LEN = 10000;
    struct Ciconv {
        iconv_t m_iconv;
        Ciconv(const iconv_t& iconv) : m_iconv(iconv) {}
        ~Ciconv()
        {
            iconv_close(m_iconv);
        }
    };

    HRESULT ErrnoToHResult(int& err) 
    {
        HRESULT hr;
        switch (err)
        {
            case 0:
                return S_OK;
            case E2BIG:
                return E_BOUNDS;
            case EILSEQ:
                return E_INVALIDARG;
            case EINVAL:
                return E_INVALIDARG;
        }
        return E_FAIL;
    }

    HRESULT SystemString::Convert(_In_ const CHAR* lpzStr, _Inout_ tstring& result)
    {
        if (lpzStr == nullptr)
        {
            result = _T("");
            return E_INVALIDARG;
        }

        Ciconv icnv(iconv_open("UTF-16LE", "UTF-8"));

        // Include the null terminator.
        size_t inputLength = strnlen(lpzStr, MAX_STRING_LEN)  + 1;

        if (inputLength >= MAX_STRING_LEN)
        {
            return E_BOUNDS;
        }

        size_t outputMax = (inputLength + 1) * 4;
        unique_ptr<WCHAR[]> buffer(new WCHAR[outputMax]);

        const char* pOrig = lpzStr;
        char* pNew = (char*)buffer.get();
        size_t count = iconv(icnv.m_iconv, (char**)&pOrig, &inputLength, &pNew, &outputMax);
        if (count == (size_t)-1)
        {
            HRESULT hr = ErrnoToHResult(errno);
            result = u"";
            return hr;
        }

        result = buffer.get();
        return S_OK;
    }

    HRESULT SystemString::Convert(_In_ const WCHAR* lpzwStr, _Inout_ string& result)
    {
        if (lpzwStr == nullptr)
        {
            result = "";
            return E_INVALIDARG;
        }

        size_t i = 0;
        const WCHAR* p = lpzwStr;
        // scan to a resonable length for the end of the string.
        for (i = 0; i < MAX_STRING_LEN; ++i, ++p)
        {
            if (*p == (WCHAR)0)
            {
                break;
            }
        }

        if (i >= MAX_STRING_LEN)
        {
            result = "";
            return E_BOUNDS;
        }


        Ciconv icnv(iconv_open("UTF-8", "UTF-16LE"));

        // include the null terminator in the input string.
        size_t inbytes = (i+1)*2;
        size_t outbytes = (i+1)*4;
        unique_ptr<char[]> buffer(new char[outbytes]);
        char* input = (char*)lpzwStr;
        char* output = buffer.get();
        size_t count = iconv(icnv.m_iconv, &input, &inbytes, &output, &outbytes);
        if (count == (size_t)-1)
        {
            result = "";
            return ErrnoToHResult(errno);
        }

        result = buffer.get();
        return S_OK;
    }
#else


    HRESULT SystemString::Convert(_In_ const WCHAR* lpzwStr, _Inout_ string& result)
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

    HRESULT SystemString::Convert(_In_ const CHAR* lpzStr, _Inout_ tstring& result)
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