// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Utilites for accessing OS info
#include "stdafx.h"
#include "SystemUtils.h"
#include <mutex>

HRESULT NaglerSystemUtils::GetEnvVar(_In_z_ const WCHAR* wszVar, _Inout_ tstring& value)
{
    HRESULT hr = E_FAIL;
    const size_t buffSize = 1024;
    #ifdef PLATFORM_UNIX
    // getenv() can cause issues when it is called
    // by multiple threads at the same time.
    // Note, this isn't perfect because there is nothing stopping
    // other libraries from calling getenv(), but it should be fine
    // for our usage.
    static std::mutex cs;
    std::lock_guard<mutex> lock(cs);
    string utfVar;
    IfFailRet(SystemString::Convert(wszVar, utfVar));
    char* val = getenv(utfVar.c_str());
    if (val != nullptr)
    {

        auto buff = std::make_unique<char[]>(buffSize);
        strncpy(buff.get(), val, buffSize);
        const void* end = memchr(buff.get(), '\0', buffSize);
        if (end == nullptr)
        {
            return E_FAIL;
        }

        return SystemString::Convert(buff.get(), value);
    }
    return E_FAIL;
    #else
    auto buff = std::make_unique<WCHAR[]>(buffSize);
    DWORD size = GetEnvironmentVariableW(wszVar, buff.get(), buffSize);
    if ((size == 0) || ((size_t)size) >= buffSize) 
    {
        return E_FAIL;
    }
    value = buff.get();
    return S_OK;
    #endif
} 
