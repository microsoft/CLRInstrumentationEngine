// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifndef IfFailRet_Proxy
#define IfFailRet_Proxy(EXPR) \
    do { if (FAILED(hr = (EXPR))) { eventLogger.LogError(_T("IfFailRet(") _T(#EXPR) _T(") failed in function ") __FUNCTIONT__); return hr; } } while (false)
#endif

#ifndef IfFailRetErrno_Proxy
#define IfFailRetErrno_Proxy(EXPR) \
    do { errno_t ifFailRetErrno_result = EXPR; IfFailRet_Proxy(MAKE_HRESULT_FROM_ERRNO(ifFailRetErrno_result)); } while (false)
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers


#ifdef PLATFORM_UNIX
#error "Only Windows is supported for the profiler proxy"
#else 
#include <windows.h>
#include <atlbase.h>
#include <atlcom.h>
#endif


#include "../Common.Headers/tstring.h"
#include "../Common.Headers/CriticalSectionHolder.h"
#include "../Common.Headers/InitOnce.h"
#include "../Common.Headers/Singleton.h"
#include "../Common.Headers/SafeFindFileHandle.h"
#include "../Common.Headers/banned.h"

#include "../Common.Headers/Macros.h"
#include "../Common.Lib/EventLoggingBase.h"
