// Copyright (c) Microsoft Corporation. All rights reserved.
// 

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef PLATFORM_UNIX
#include "unix.h"
#include <ole.h>
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
#include <corprof.h>
#include <sal.h>

#ifndef PLATFORM_UNIX
#include <msxml6.h>
#include <Pathcch.h>
#endif


#ifdef PLATFORM_UNIX
#include <string>
#endif

#include "tstring.h"

#include "Macros.h"

#include <vector>
#include <memory>
#include <unordered_map>

#ifdef PLATFORM_UNIX
// pal.h defines these, but they aren't picked up for our build because std_c++ compatibility is defined
// in CMakeFile.txt
#define fclose        PAL_fclose
#define fflush        PAL_fflush
#define fprintf       PAL_fprintf
#define fwprintf      PAL_fwprintf
#endif

#include "CriticalSectionHolder.h"
#include "Logging.h"

using namespace std;
using namespace MicrosoftInstrumentationEngine;
