// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <sys/stat.h>

extern "C" BOOL __stdcall PathFileExistsW(LPCWSTR pszPath)
{
    // todo: do we ever need to replace '\' with '/'?
    CHAR mbUnixFileName[_MAX_PATH];
    struct stat stat_data;

    if (pszPath == NULL)
    {
        return FALSE;
    }

    // Convert the parameters to ASCII and then use stat
    ATL::CW2A str(pszPath);
    return (stat(str, &stat_data) == 0);
}

extern "C" LPCWSTR __stdcall PathFindExtensionW(LPCWSTR pszPath)
{
    LPCWSTR lastDot = NULL;
    if (pszPath)
    {
        while (*pszPath)
        {
            if (*pszPath == '.')
            {
                lastDot = pszPath;
            }
            else if (*pszPath == '/')
            {
                // note windows does not allow spaces in extension, but linux doesn't care
                lastDot = NULL;
            }
            pszPath++;
        }
    }
    if (lastDot)
    {
        return lastDot;
    }
    return pszPath;
}

// TODO: Fix linkage of this function in PAL and remove this
extern "C" LPWSTR PAL_PathFindFileNameW(LPCWSTR pPath)
{
    LPWSTR ret = (LPWSTR)pPath;
    if (ret != NULL && *ret != W('\0'))
    {
        ret = PAL_wcschr(ret, W('\0')) - 1;
        if (ret > pPath && *ret == W('/'))
        {
            ret--;
        }
        while (ret > pPath && *ret != W('/'))
        {
            ret--;
        }
        if (*ret == W('/') && *(ret + 1) != W('\0'))
        {
            ret++;
        }
    }

    return ret;
}

extern "C" HRESULT PathCchAppend(
    _Inout_  WCHAR*  wszPath,
    _In_     size_t cchPath,
    _In_opt_ WCHAR* wszMore
)
{
    HRESULT hr = S_OK;

    if (wszPath == nullptr)
    {
        return E_INVALIDARG;
    }

    if (wszMore == nullptr)
    {
        return E_INVALIDARG;
    }

    size_t cchPathActual = PAL_wcslen(wszPath);
    size_t cchMore = PAL_wcslen(wszMore);

    size_t cchNeeded = cchPathActual + cchMore;

    if (wszPath[cchPathActual - 1] != u'/')
    {
        cchNeeded++;
    }

    if (cchNeeded > cchPath)
    {
        const HRESULT PATHCCH_E_FILENAME_TOO_LONG = (HRESULT)0x800700CEL;
        return PATHCCH_E_FILENAME_TOO_LONG;
    }

    if (wszPath[cchPathActual - 1] != _T('/'))
    {
        PAL_wcscat(wszPath, _T("/"));
    }

    PAL_wcscat(wszPath, wszMore);

    return S_OK;
}

extern "C"  HRESULT PathCchRemoveFileSpec(
    _Inout_ WCHAR*  wszPath,
    _In_    size_t cchPath
)
{
    if (wszPath == nullptr)
    {
        return E_INVALIDARG;
    }

    // Similar to windows' PathCchRemoveFileSpec, this removes the file from a path.

    WCHAR* pLastSlash = PAL_wcsrchr(wszPath, u'/');
    if (pLastSlash != nullptr)
    {
        *pLastSlash = u'\0';
    }

    return S_OK;
}

