// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

namespace MicrosoftInstrumentationEngine
{
    // for exporting __stdcall/__fastcall methods to a known name. ARGSIZE is in bytes.
    #ifdef _WIN64
    #define DLLEXPORT(METHOD, ARGSIZE) __pragma(comment(linker, "/EXPORT:" #METHOD ",PRIVATE")) METHOD
    #else
    #define DLLEXPORT(METHOD, ARGSIZE) __pragma(comment(linker, "/EXPORT:" #METHOD "=_" #METHOD "@" #ARGSIZE ",PRIVATE")) METHOD
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

#ifndef IfNullRetPointer
#define IfNullRetPointer(EXPR) \
    do { if (NULL == (EXPR)) { CLogging::LogError(_T(#EXPR) _T(" is null in function ") __FUNCTIONT__); return E_POINTER; } } while (false)
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

    template<typename T, int size>
    int array_length(T(&)[size]){return size;}
}