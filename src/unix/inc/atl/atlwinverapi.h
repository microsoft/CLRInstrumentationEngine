// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLWINVERAPI_H__
#define __ATLWINVERAPI_H__

#pragma once

#ifdef _ATL_ALL_WARNINGS
#pragma warning( push )
#endif

#pragma warning(disable: 4786) // identifier was truncated in the debug information
#pragma warning(disable: 4127) // constant expression
#pragma warning(disable: 4949) // pragmas 'managed' and 'unmanaged' are meaningful only when compiled with /clr

#include <atldef.h>
#include <windows.h>
#include <ole2.h>
#include <sdkddkver.h>

// This file contains declarations of wrappers for methods used
// in ATL that are only available in later versions of Windows.

// When the minimum supported version of Windows is increased, the
// implementations of these methods just call the real Windows APIs.

// Minimum supported versions of Windows:
// Windows XP SP2 for x86 and x64, Windows 8 for ARM

#if defined(_M_IX86) || defined(_M_X64)
#define _ATL_NTDDI_MIN NTDDI_WINXPSP2
#else
#define _ATL_NTDDI_MIN NTDDI_WIN8
#endif

// Use this macro for loading a local cached function from a DLL that is known to be loaded (e.g. KERNEL32)
#define IFDYNAMICGETCACHEDFUNCTION(libraryname, functionname, functionpointer) \
	static volatile auto functionpointer##_cache = reinterpret_cast<decltype(::functionname)*>(NULL); \
	auto functionpointer = reinterpret_cast<decltype(::functionname)*>(functionpointer##_cache); \
	if (functionpointer == reinterpret_cast<decltype(::functionname)*>(NULL)) \
	{ \
		HINSTANCE hLibrary = GetModuleHandleW(libraryname); \
		if (hLibrary != NULL) \
		{ \
			functionpointer = reinterpret_cast<decltype(::functionname)*>(::GetProcAddress(hLibrary, #functionname)); \
			functionpointer##_cache = reinterpret_cast<decltype(::functionname)*>(::EncodePointer((PVOID)functionpointer)); \
		} \
	} \
	else \
	{ \
		functionpointer = reinterpret_cast<decltype(::functionname)*>(::DecodePointer((PVOID)functionpointer)); \
	} \
	if (functionpointer != reinterpret_cast<decltype(::functionname)*>(NULL))

#pragma pack(push,_ATL_PACKING)
namespace ATL
{
#pragma managed(push, off)
#ifndef _ATL_STATIC_LIB_IMPL
    extern inline BOOL __cdecl _AtlInitializeCriticalSectionEx(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount, DWORD Flags)
    {
#if (NTDDI_VERSION >= NTDDI_VISTA) && !defined(_USING_V110_SDK71_) && !defined(_ATL_XP_TARGETING)
        // InitializeCriticalSectionEx is available in Vista or later, desktop or store apps
        return ::InitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags);
#else
        UNREFERENCED_PARAMETER(Flags);

        // ...otherwise fall back to using InitializeCriticalSectionAndSpinCount.
        return ::InitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount);
#endif
    }
#else
    BOOL __cdecl _AtlInitializeCriticalSectionEx(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount, DWORD Flags);
#endif
#pragma managed(pop)

    // Needed for downlevel NLS APIs
    LCID __cdecl _AtlDownlevelLocaleNameToLCID(LPCWSTR localeName);
    int __cdecl _AtlDownlevelLCIDToLocaleName(LCID lcid, LPWSTR outLocaleName, int cchLocaleName);

}	// namespace ATL
#pragma pack(pop)

#ifdef _ATL_ALL_WARNINGS
#pragma warning( pop )
#endif

#endif	// __ATLWINVERAPI_H__
