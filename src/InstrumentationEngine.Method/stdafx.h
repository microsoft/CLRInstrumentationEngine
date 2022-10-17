// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifdef PLATFORM_UNIX
#include "unix.h"
#include <ole.h>
//#include "corhdr.h"
//#include "CorHeaders.h"
#include <palrt.h>
#include <pal.h>
#endif

#include "targetver.h"
#ifndef PLATFORM_UNIX
#include <windows.h>
#endif
#include "string.h"


#include <errorrep.h>       // for pfn_REPORTFAULT
#include <eh.h>

#include <sal.h>

#include <atlcomcli.h>
#ifndef PLATFORM_UNIX
#include <atlbase.h>
#include <atlcom.h>
#include <atlutil.h>
#endif
#include <atlcore.h>
#include <atlcoll.h>
#include <atlsync.h>

using namespace ATL;

#include <strsafe.h>
#include <cor.h>
#include <sal.h>

#ifndef PLATFORM_UNIX
#include <msxml6.h>
#endif

#ifdef PLATFORM_UNIX
#include <string>
#endif

#ifdef PLATFORM_UNIX
// pal.h defines these, but they aren't picked up for our build because std_c++ compatibility is defined
// in CMakeFile.txt
#define wcsstr        PAL_wcsstr
#define wcscmp        PAL_wcscmp
#endif

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <inttypes.h>
#include <stdint.h>

#include "../InstrumentationEngine.Api/InstrumentationEngine.h"

#ifndef IfFailRet
#define IfFailRet(EXPR) \
do { if (FAILED(hr = (EXPR))) { ATLASSERT(!L"IfFailRet(" L#EXPR L") failed"); return hr; } } while (false)
#endif

using namespace std;