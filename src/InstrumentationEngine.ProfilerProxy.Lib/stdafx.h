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

#include "../Common.Lib/tstring.h"
#include "../Common.Lib/CriticalSectionHolder.h"
#include "../Common.Lib/InitOnce.h"
#include "../Common.Lib/Singleton.h"
#include "../Common.Lib/Macros.h"
#include "../Common.Lib/EventLoggingBase.h"
#include "../Common.Lib/SafeFindFileHandle.h"

#include "..//Common.Lib/banned.h"