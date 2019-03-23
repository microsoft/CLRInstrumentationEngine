// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#ifdef PLATFORM_UNIX
#include "unix.h"
#include <ole.h>
#endif

#include "targetver.h"
#ifndef PLATFORM_UNIX
#include <windows.h>
#endif
#include "string.h"
//#include <vsassert.h>


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
#include "CorHeaders.h"
#include <sal.h>

#include "Macros.h"

#include <vector>
#include <memory>
#include <unordered_map>

#include "CriticalSectionHolder.h"

using namespace std;

#include "refcount.h"
#include "ImplQueryInterface.h"
#include "InstrumentationEngine.h"

#include "Logging.h"
#include "tstring.h"
