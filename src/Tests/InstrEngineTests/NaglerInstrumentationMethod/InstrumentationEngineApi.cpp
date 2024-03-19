// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "InstrumentationEngineApi.h"
#include "../Common.Headers/InitOnce.h"

#if defined(PLATFORM_UNIX)

#include <dlfcn.h>

#if (defined(_M_X64) || defined(_M_ARM64) || defined(_M_AMD64))
const char InstrumentationEngineModule[] = "libInstrumentationEngine.so";
#else
#error "Platform Not Supported"
#endif

#define MOD_TYPE void*
#define MOD_LOAD(_N) (dlopen(_N, RTLD_NOLOAD | RTLD_LAZY))
#define SYM_LOAD(_M, _S) (dlsym(_M, _S))
#else // !PLATFORM_UNIX
#include <Windows.h>

#if defined(X86)
const WCHAR InstrumentationEngineModule[] = _T("MicrosoftInstrumentationEngine_x86.dll");
#elif defined(X64)
const WCHAR InstrumentationEngineModule[] = _T("MicrosoftInstrumentationEngine_x64.dll");
#else
#error "Platform Not Supported"
#endif

#define MOD_TYPE HMODULE
#define MOD_LOAD(_N) (GetModuleHandle(_N))
#define SYM_LOAD(_M, _S) (GetProcAddress(_M, _S))
#define GET_LAST_ERROR() \
    
#endif

using namespace std;

namespace InstrumentationEngineApi
{

    class ApiFunctions
    {
    private:
        void AssertLastError()
        {
#ifdef PLATFORM_UNIX
            AssertFailed(dlerror());
            AssertFailed("\n");
#else
            // Aid in finding the line of code associated with the error message:
            // stderr: The specified module could not be found.
            WCHAR err[256];
            memset(err, 0, 256);
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
            LPCWSTR errMsg = err;
            AssertLogFailure(errMsg);
#endif
        }

    public:
        using TFreeString = HRESULT(STDMETHODCALLTYPE*)(BSTR bstr);
        TFreeString _InstrumentationEngineFreeString = nullptr;
        
        HRESULT Initialize()
        {
            MOD_TYPE mod = MOD_LOAD(InstrumentationEngineModule);
            if (mod == nullptr)
            {
                AssertLastError();
                return E_NOTIMPL;
            }

            _InstrumentationEngineFreeString = (TFreeString)SYM_LOAD(mod, "InstrumentationEngineFreeString");
            if (_InstrumentationEngineFreeString == nullptr)
            {
                AssertLastError();
                return E_NOTIMPL;
            }

            return S_OK;
        }
    };

    static InstrumentationEngineApi::ApiFunctions g_apiFunctions;
    static CInitOnce g_initializer([] { return g_apiFunctions.Initialize(); });

    HRESULT FreeString(BSTR bstr)
    {
        HRESULT hr;
        IfFailRet(g_initializer.Get());
        return g_apiFunctions._InstrumentationEngineFreeString(bstr);
    }
}
