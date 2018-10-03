// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Headers for CppUnitTest
#include "CppUnitTest.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers



#include <windows.h>
#include "string.h"

#include <errorrep.h>       // for pfn_REPORTFAULT
#include <eh.h>


#include <sal.h>

#include <atlbase.h>
#include <atlcom.h>
#include <atlutil.h>
#include <atlcoll.h>
#include <atlsync.h>

#include <strsafe.h>
#include <cor.h>
#include <corprof.h>
#include <sal.h>

#include <msxml6.h>

#include "Macros.h"

#include <vector>
#include <memory>
#include <unordered_map>

#include "CriticalSectionHolder.h"
#include "Logging.h"

using namespace std;
using namespace MicrosoftInstrumentationEngine;
