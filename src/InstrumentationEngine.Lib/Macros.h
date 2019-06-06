// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

namespace MicrosoftInstrumentationEngine
{
// for exporting __stdcall/__fastcall methods to a known name. ARGSIZE is in bytes.
#ifndef PLATFORM_UNIX
#ifdef _WIN64
#define DLLEXPORT(METHOD, ARGSIZE) __pragma(comment(linker, "/EXPORT:" #METHOD ",PRIVATE")) METHOD
#else
#define DLLEXPORT(METHOD, ARGSIZE) __pragma(comment(linker, "/EXPORT:" #METHOD "=_" #METHOD "@" #ARGSIZE ",PRIVATE")) METHOD
#endif
#else
#define DLLEXPORT(METHOD, ARGSIZE) __attribute__((visibility("default"))) METHOD
#endif

#ifndef PLATFORM_UNIX
#define __FUNCTIONT__ _T(__FUNCTION__)
#else
    // In clang __FUNCTION__ cannot be used as a string literal so we cannot concatenate it in the
    // preprocessor (compiler?) like we want to. For now just don't output function name.
#define __FUNCTIONT__
#endif

#ifndef IfFailRet
#define IfFailRet(EXPR) \
    do { if (FAILED(hr = (EXPR))) { CLogging::LogError(_T("IfFailRet(") _T(#EXPR) _T(") failed in function ") __FUNCTIONT__); return hr; } } while (false)
#endif

#ifndef IfFailRet_Proxy
#define IfFailRet_Proxy(EXPR) \
    do { if (FAILED(hr = (EXPR))) { CProxyLogging::LogError(_T("IfFailRet(") _T(#EXPR) _T(") failed in function ") __FUNCTIONT__); return hr; } } while (false)
#endif

#ifndef IfFailRetNoLog
#define IfFailRetNoLog(EXPR) \
    do { if (FAILED(hr = (EXPR))) { return hr; } } while (false)
#endif

// Wrap errno_t result of EXPR in HRESULT and then IfFailRet.
#ifndef IfFailRetErrno
#define IfFailRetErrno(EXPR) \
    do { errno_t ifFailRetErrno_result = EXPR; IfFailRet(MAKE_HRESULT_FROM_ERRNO(ifFailRetErrno_result)); } while (false)
#endif

#ifndef IfFailRetErrno_Proxy
#define IfFailRetErrno_Proxy(EXPR) \
    do { errno_t ifFailRetErrno_result = EXPR; IfFailRet_Proxy(MAKE_HRESULT_FROM_ERRNO(ifFailRetErrno_result)); } while (false)
#endif

#ifndef IfFailRetNoLog
#define IfFailRetNoLog(EXPR) \
    do { if (FAILED(hr = (EXPR))) { return hr; } } while (false)
#endif

// Treats errno 0 as severity=SUCCESS others to severity=ERROR, sets facility to null, and code to 16 least-significant bits of errno.
#ifndef MAKE_HRESULT_FROM_ERRNO
#define MAKE_HRESULT_FROM_ERRNO(errnoValue) \
    (MAKE_HRESULT(errnoValue == 0 ? SEVERITY_SUCCESS : SEVERITY_ERROR, FACILITY_NULL, HRESULT_CODE(errnoValue)))
#endif

#ifndef IfNullRetPointer
#define IfNullRetPointer(EXPR) \
    do { if (NULL == (EXPR)) { CLogging::LogError(_T(#EXPR) _T(" is null in function ") __FUNCTIONT__); return E_POINTER; } } while (false)
#endif

#ifndef IfNullRetPointerNoLog
#define IfNullRetPointerNoLog(EXPR) \
    do { if (NULL == (EXPR)) { return E_POINTER; } } while (false)
#endif

#ifndef IfNullRet
#define IfNullRet(EXPR) \
    do { if (NULL == (EXPR)) { CLogging::LogError(_T(#EXPR) _T(" is null in function ") __FUNCTIONT__); return E_FAIL; } } while (false)
#endif

#ifndef IfFalseRet
#define IfFalseRet(EXPR) \
    do { if (FALSE == (EXPR)) { CLogging::LogError(_T("IfFalseRet(") _T(#EXPR) _T(") failed in function ") __FUNCTIONT__); return E_FAIL; } } while (false)
#endif

#ifndef IsFlagSet
    #define IsFlagSet(dwAllFlags, dwFlag) (((dwAllFlags) & (dwFlag)) != 0)
#endif

#ifndef IsFlagClear
    #define IsFlagClear(dwAllFlags, dwFlag) (((dwAllFlags) & (dwFlag)) == 0)
#endif

#ifndef IfNotInitRet
#define IfNotInitRet(initOnce) \
    do { if (!initOnce.IsSuccessful()) { return; } } while (false)
#endif

#ifndef IfNotInitRetFalse
#define IfNotInitRetFalse(initOnce) \
    do { if (!initOnce.IsSuccessful()) { return false; } } while (false)
#endif

#ifndef IfNotInitRetUnexpected
#define IfNotInitRetUnexpected(initOnce) \
    do { if (!initOnce.IsSuccessful()) { return E_UNEXPECTED; } } while (false)
#endif

    template<typename T, int size>
    int array_length(T(&)[size]){return size;}
}