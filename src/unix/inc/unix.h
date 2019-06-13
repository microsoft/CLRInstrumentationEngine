// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#ifndef __UNIX_H__
#define __UNIX_H__

typedef __builtin_va_list va_list;
#define interface struct
#define WINBASEAPI DECLSPEC_IMPORT
#define DECLSPEC_SELECTANY  __attribute__((weak))
#define DECLSPEC_ALIGN(x)   __declspec(align(x))
#define DECLSPEC_NOTHROW   __declspec(nothrow)

#define DECLSPEC_NOVTABLE   __declspec(novtable)
#define DECLSPEC_IMPORT     __declspec(dllimport)
#define DECLSPEC_SELECTANY  __declspec(selectany)
#define _DECLSPEC_DEFINED_

#define IID_NULL GUID_NULL
#define min_value(a,b)            (((a) < (b)) ? (a) : (b))
#define max_value(a,b)                  (((a) > (b)) ? (a) : (b))
#define DIRECTORY_SEPERATOR_CHAR _T('/')
#define DIRECTORY_SEPERATOR_STR  _T("/")
#define PATH_LIST_SEPERATOR_CHAR _T(':')
#define PATH_LIST_SEPERATOR_STR  _T(":")

#define PUBLIC_EXPORT __attribute__((visibility("default")))

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "no_sal.h"
#include "no_sal2.h"

// The PAL's .a exports _vsnprintf with the 'PAL__' prefix. But it skips this #define for us
// because we set PAL_STDCPP_COMPAT. Turn on just this one that we need.
#define _vsnprintf    PAL__vsnprintf

// PAL redefines is only strictly necessary for the Alpine scenario,
// but we do it for all Linux builds for simplicity's sake.
#ifdef _LINUX_
#include "pal_redefines.h"
#endif

#include "pal.h"
#include "palrt.h"
#include "servprov.h"

#include "palrt2.h"

#include "oaidl.h"
#include "ole.h"
#include "Dbghelp.h"

#ifdef __cplusplus

// Define operator overloads to enable bit operations on enum values that are
// used to define flags. Use DEFINE_ENUM_FLAG_OPERATORS(YOUR_TYPE) to enable these
// operators on YOUR_TYPE.

// Moved here from objbase.w.

#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) \
extern "C++" { \
inline ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((int)a) | ((int)b)); } \
inline ENUMTYPE &operator |= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((int &)a) |= ((int)b)); } \
inline ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((int)a) & ((int)b)); } \
inline ENUMTYPE &operator &= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((int &)a) &= ((int)b)); } \
inline ENUMTYPE operator ~ (ENUMTYPE a) { return ENUMTYPE(~((int)a)); } \
inline ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((int)a) ^ ((int)b)); } \
inline ENUMTYPE &operator ^= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((int &)a) ^= ((int)b)); } \
}
#else
#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) // NOP, C allows these operators.
#endif

#define INTERNET_MAX_PATH_LENGTH 2048 // Defined on Windows in wininet.h

#ifndef MAXWORD
#define MAXWORD 0xffff
#endif

#endif // __UNIX_H__
