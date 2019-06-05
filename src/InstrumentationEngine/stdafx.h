// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#ifdef PLATFORM_UNIX
#include "unix.h"
#include <ole.h>
#include "corhdr.h"
#include "CorHeaders.h"
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
#include <corprof.h>
#include "CorHeaders.h"
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
#endif

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "InstrumentationEngine.h"
#include "../InstrumentationEngine.Lib/ImplQueryInterface.h"
#include "../InstrumentationEngine.Lib/Logging.h"
#include "../InstrumentationEngine.Lib/refcount.h"

#include "resource.h"

#include "../Common.Lib/tstring.h"
#include "../Common.Lib/Macros.h"
#include "../Common.Lib/CriticalSectionHolder.h"

#include "../Common.Lib/banned.h"

using namespace std;