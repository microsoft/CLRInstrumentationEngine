// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once



#ifdef PLATFORM_UNIX
#include "unix.h"
#include <ole.h>
#include <palrt.h>
#include <pal.h>
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
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
//#ifndef PLATFORM_UNIX
#include <atlcoll.h>
//#endif
#include <atlsync.h>

using namespace ATL;

#include <strsafe.h>
#include <cor.h>
#include <clrprofiler.h>
#include <sal.h>

#ifndef PLATFORM_UNIX
#include <Shlwapi.h>
#endif

#ifdef PLATFORM_UNIX
#include <string>
#endif

#ifdef PLATFORM_UNIX
// pal.h defines these, but they aren't picked up for our build because std_c++ compatibility is defined
// in CMakeFile.txt
#define PAL_wcsnlen_s(a, b) (a != nullptr) ? PAL_wcsnlen(a, b) : 0
#define wcsstr        PAL_wcsstr
#define wcslen        PAL_wcslen
#define wcsnlen_s     PAL_wcsnlen_s
#define wcsstr        PAL_wcsstr
#define wcscmp        PAL_wcscmp
#endif

#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy can be used
#pragma warning(disable: 6285) // bug in <functional> that compares 0 to 0, resulting in a warning.
#include <queue>
#include <vector>
#include <memory>
#include <unordered_map>

#ifdef PLATFORM_UNIX
#define __valid_backup __valid // sal.h redefines __valid, which is used by thread/chrono
#undef __valid
#endif

#include <thread>  
#include <mutex>   

#ifdef PLATFORM_UNIX
#define __valid __valid_backup
#endif

#ifdef PLATFORM_UNIX
// pal.h defines these, but they aren't picked up for our build because std_c++ compatibility is defined
// in CMakeFile.txt
#define fclose        PAL_fclose
#define fflush        PAL_fflush
#define fprintf       PAL_fprintf
#define fwprintf      PAL_fwprintf
#endif

#include <time.h>
#include <iomanip>
#include <wchar.h>
#ifdef PLATFORM_UNIX
#include <ctime>
#endif

#include <functional>

#pragma warning(pop)

#include "Logging.h"
#include "../Common.Lib/ImplQueryInterface.h"
#include "../Common.Lib/refcount.h"
#include "SharedArray.h"

#include "../Common.Headers/tstring.h"
#include "../Common.Lib/Macros.h"
#include "../Common.Headers/CriticalSectionHolder.h"
#include "../Common.Headers/InitOnce.h"
#include "../Common.Headers/Singleton.h"

#include "../Common.Headers/banned.h"

using namespace std;
using namespace MicrosoftInstrumentationEngine;