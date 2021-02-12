// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

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

// Asserts and optionally logs that a failure occured with the given hresult.
// Note, this library does not define the implementation of this function. It
// is up to the binary executable that links this library to supply a definition
// otherwise, linkage errors will be expected.
void AssertLogFailure(_In_ const WCHAR* wszError, ...);

    // Log an error without returning.
#ifndef IfFailLog
#define IfFailLog(EXPR) \
    do { if (FAILED(hr = (EXPR))) { AssertLogFailure(_T("IfFailLog(") _T(#EXPR) _T(") failed in function ") __FUNCTIONT__ _T(": %d"), hr); } } while (false)
#endif

#ifndef IfFailRet
#define IfFailRet(EXPR) \
    do { if (FAILED(hr = (EXPR))) { AssertLogFailure(_T("IfFailRet(") _T(#EXPR) _T(") failed in function ") __FUNCTIONT__); return hr; } } while (false)
#endif

// Wrap errno_t result of EXPR in HRESULT and then IfFailRet.
#ifndef IfFailRetErrno
#define IfFailRetErrno(EXPR) \
    do { errno_t ifFailRetErrno_result = EXPR; IfFailRet(MAKE_HRESULT_FROM_ERRNO(ifFailRetErrno_result)); } while (false)
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
    do { if (NULL == (EXPR)) { AssertLogFailure(_T(#EXPR) _T(" is null in function ") __FUNCTIONT__); return E_POINTER; } } while (false)
#endif

#ifndef IfNullRetPointerNoLog
#define IfNullRetPointerNoLog(EXPR) \
    do { if (NULL == (EXPR)) { return E_POINTER; } } while (false)
#endif

#ifndef IfNullRet
#define IfNullRet(EXPR) \
    do { if (NULL == (EXPR)) { AssertLogFailure(_T(#EXPR) _T(" is null in function ") __FUNCTIONT__); return E_FAIL; } } while (false)
#endif

#ifndef IfFalseRet
#define IfFalseRet(EXPR, HR) \
    do { if (FALSE == (EXPR)) { AssertLogFailure(_T("IfFalseRet(") _T(#EXPR) _T(") failed in function ") __FUNCTIONT__); return (HR); } } while (false)
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