// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// ported from coreclr\pal\inc\strsafe.h

#include "pch.h"

#define STRSAFEAPI          HRESULT __cdecl
#define STRSAFEWORKERAPI    static STRSAFEAPI

#define STRSAFE_MAX_CCH  2147483647 // max # of characters we support (same as INT_MAX)
#define STRSAFE_MAX_LENGTH  (STRSAFE_MAX_CCH - 1)   // max buffer length, in characters, that we support

typedef LONG HRESULT;

// STRSAFE error return codes
//
#define STRSAFE_E_INSUFFICIENT_BUFFER       ((HRESULT)0x8007007AL)  // 0x7A = 122L = ERROR_INSUFFICIENT_BUFFER
#define STRSAFE_E_INVALID_PARAMETER         ((HRESULT)0x80070057L)  // 0x57 =  87L = ERROR_INVALID_PARAMETER
#define STRSAFE_E_END_OF_FILE               ((HRESULT)0x80070026L)  // 0x26 =  38L = ERROR_HANDLE_EOF

#ifndef SUCCEEDED
#define SUCCEEDED(hr)  ((HRESULT)(hr) >= 0)
#endif

#ifndef FAILED
#define FAILED(hr)  ((HRESULT)(hr) < 0)
#endif

#ifndef S_OK
#define S_OK  ((HRESULT)0x00000000L)
#endif

int _vsnprintf_unsafe(char *_Dst, size_t _SizeInWords, size_t _Count, const char *_Format, va_list _ArgList);

#pragma warning(push)
#pragma warning(disable : 4100) // Unused parameter (pszDest)
_Post_satisfies_(cchDest > 0 && cchDest <= cchMax)
STRSAFEWORKERAPI
    StringValidateDestA(
        _In_reads_opt_(cchDest) char* pszDest,
        _In_ size_t cchDest,
        _In_ const size_t cchMax)
{
    HRESULT hr = S_OK;

    if ((cchDest == 0) || (cchDest > cchMax))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }

    return hr;
}
#pragma warning(pop)

STRSAFEWORKERAPI
StringValidateDestW(
    _In_reads_opt_(cchDest) WCHAR* pszDest,
    _In_ size_t cchDest,
    _In_ const size_t cchMax)
{
    HRESULT hr = S_OK;

    if ((cchDest == 0) || (cchDest > cchMax))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }

    return hr;
}

STRSAFEWORKERAPI
StringLengthWorkerW(
    _In_reads_or_z_(cchMax) const WCHAR* psz,
    _In_ _In_range_(<= , STRSAFE_MAX_CCH) size_t cchMax,
    _Out_opt_ _Deref_out_range_(<, cchMax) size_t* pcchLength)
{
    HRESULT hr = S_OK;
    size_t cchOriginalMax = cchMax;

    while (cchMax && (*psz != u'\0'))
    {
        psz++;
        cchMax--;
    }

    if (cchMax == 0)
    {
        // the string is longer than cchMax
        hr = STRSAFE_E_INVALID_PARAMETER;
    }

    if (pcchLength)
    {
        if (SUCCEEDED(hr))
        {
            *pcchLength = cchOriginalMax - cchMax;
        }
        else
        {
            *pcchLength = 0;
        }
    }

    return hr;
}

_Post_satisfies_(cchDest > 0 && cchDest <= cchMax)
STRSAFEWORKERAPI
StringValidateDestAndLengthW(
    _In_reads_opt_(cchDest) WCHAR* pszDest,
    _In_ size_t cchDest,
    _Out_ _Deref_out_range_(0, cchDest - 1) size_t* pcchDestLength,
    _In_ const size_t cchMax)
{
    HRESULT hr;

    hr = StringValidateDestW(pszDest, cchDest, cchMax);

    if (SUCCEEDED(hr))
    {
        hr = StringLengthWorkerW(pszDest, cchDest, pcchDestLength);
    }
    else
    {
        *pcchDestLength = 0;
    }

    return hr;
}

STRSAFEWORKERAPI
    StringCopyWorkerW(
    _Out_writes_(cchDest) _Always_(_Post_z_) WCHAR* pszDest,
    _In_ _In_range_(1, STRSAFE_MAX_CCH) size_t cchDest,
    _Out_opt_ _Deref_out_range_(<= , (cchToCopy < cchDest) ? cchToCopy : (cchDest - 1)) size_t* pcchNewDestLength,
    _In_reads_or_z_(cchToCopy) const WCHAR* pszSrc,
    _In_ _In_range_(<, STRSAFE_MAX_CCH) size_t cchToCopy)
{
    HRESULT hr = S_OK;
    size_t cchNewDestLength = 0;

    // ASSERT(cchDest != 0);

    while (cchDest && cchToCopy && (*pszSrc != u'\0'))
    {
        *pszDest++ = *pszSrc++;
        cchDest--;
        cchToCopy--;

        cchNewDestLength++;
    }

    if (cchDest == 0)
    {
        // we are going to truncate pszDest
        pszDest--;
        cchNewDestLength--;

        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    }

    *pszDest = u'\0';

    if (pcchNewDestLength)
    {
        *pcchNewDestLength = cchNewDestLength;
    }

    return hr;
}

STRSAFEWORKERAPI
StringVPrintfWorkerA(
    _Out_writes_(cchDest) _Always_(_Post_z_) char* pszDest,
    _In_ _In_range_(1, STRSAFE_MAX_CCH) size_t cchDest,
    _Out_opt_ _Deref_out_range_(<= , cchDest - 1) size_t* pcchNewDestLength,
    _In_ _Printf_format_string_ const char* pszFormat,
    _In_ va_list argList)
{
    HRESULT hr = S_OK;
    int iRet;
    size_t cchMax;
    size_t cchNewDestLength = 0;

    // leave the last space for the null terminator
    cchMax = cchDest - 1;

    iRet = _vsnprintf_s(pszDest, cchDest, cchMax, pszFormat, argList);
    // ASSERT((iRet < 0) || (((size_t)iRet) <= cchMax));

    if ((iRet < 0) || (((size_t)iRet) > cchMax))
    {
        // need to null terminate the string
        pszDest += cchMax;
        *pszDest = '\0';

        cchNewDestLength = cchMax;

        // we have truncated pszDest
        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    }
    else if (((size_t)iRet) == cchMax)
    {
        // need to null terminate the string
        pszDest += cchMax;
        *pszDest = '\0';

        cchNewDestLength = cchMax;
    }
    else
    {
        cchNewDestLength = (size_t)iRet;
    }

    if (pcchNewDestLength)
    {
        *pcchNewDestLength = cchNewDestLength;
    }

    return hr;
}

STRSAFEAPI
StringCchCopyW(
    _Out_writes_(cchDest) _Always_(_Post_z_) WCHAR *pszDest,
    _In_ size_t cchDest,
    _In_ const WCHAR *pszSrc)
{
    HRESULT hr;

    hr = StringValidateDestW(pszDest, cchDest, STRSAFE_MAX_CCH);

    if (SUCCEEDED(hr))
    {
        hr = StringCopyWorkerW(pszDest,
            cchDest,
            NULL,
            pszSrc,
            STRSAFE_MAX_LENGTH);
    }
    else if (cchDest > 0)
    {
        *pszDest = u'\0';
    }

    return hr;
}

STRSAFEAPI
StringCchCatW(
    _Inout_updates_(cchDest) _Always_(_Post_z_) WCHAR* pszDest,
    _In_ size_t cchDest,
    _In_ const WCHAR* pszSrc)
{
    HRESULT hr;
    size_t cchDestLength;

    hr = StringValidateDestAndLengthW(pszDest,
        cchDest,
        &cchDestLength,
        STRSAFE_MAX_CCH);

    if (SUCCEEDED(hr))
    {
        hr = StringCopyWorkerW(pszDest + cchDestLength,
            cchDest - cchDestLength,
            NULL,
            pszSrc,
            STRSAFE_MAX_LENGTH);
    }

    return hr;
}

STRSAFEAPI
StringCchPrintfA(
    _Out_writes_(cchDest) _Always_(_Post_z_) char* pszDest,
    _In_ size_t cchDest,
    _In_ const char* pszFormat,
    ...)
{
    HRESULT hr;

    hr = StringValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH);

    if (SUCCEEDED(hr))
    {
        va_list argList;

        va_start(argList, pszFormat);

        hr = StringVPrintfWorkerA(pszDest,
            cchDest,
            NULL,
            pszFormat,
            argList);

        va_end(argList);
    }
    else if (cchDest > 0)
    {
        *pszDest = '\0';
    }

    return hr;
}

STRSAFEAPI
StringCchVPrintfA(
    STRSAFE_LPSTR pszDest,
    size_t cchDest,
    STRSAFE_LPCSTR pszFormat,
    va_list argList)
{
    HRESULT hr;

    hr = StringValidateDestA(pszDest, cchDest, STRSAFE_MAX_CCH);

    if (SUCCEEDED(hr))
    {
        hr = StringVPrintfWorkerA(pszDest,
                cchDest,
                NULL,
                pszFormat,
                argList);
    }
    else if (cchDest > 0)
    {
        *pszDest = '\0';
    }

    return hr;
}

static HRESULT StringLengthWorkerA(const char* psz, size_t cchMax, size_t* pcch)
{
    HRESULT hr = S_OK;
    size_t cchMaxPrev = cchMax;

    while (cchMax && (*psz != '\0'))
    {
        psz++;
        cchMax--;
    }

    if (cchMax == 0)
    {
        // the string is longer than cchMax
        hr = STRSAFE_E_INVALID_PARAMETER;
    }

    if (SUCCEEDED(hr) && pcch)
    {
        *pcch = cchMaxPrev - cchMax;
    }

    return hr;
}

STRSAFEAPI StringCbLengthA(const char* psz, size_t cbMax, size_t* pcb)
{
    HRESULT hr;
    size_t cchMax;
    size_t cch = 0;

    cchMax = cbMax / sizeof(char);

    if ((psz == NULL) || (cchMax > STRSAFE_MAX_CCH))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringLengthWorkerA(psz, cchMax, &cch);
    }

    if (SUCCEEDED(hr) && pcb)
    {
        // safe to multiply cch * sizeof(char) since cch < STRSAFE_MAX_CCH and sizeof(char) is 1
        *pcb = cch * sizeof(char);
    }

    return hr;
}
