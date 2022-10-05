// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers


#ifdef PLATFORM_UNIX
#error "Only Windows is supported for the Profiler Proxy"
#else 
#include <windows.h>
#include <atlbase.h>
#include <atlcom.h>
#endif

#include "../Common.Headers/InitOnce.h"
#include "../Common.Headers/Singleton.h"
#include "../Common.Headers/banned.h"

#include "../Common.Lib/Macros.h"