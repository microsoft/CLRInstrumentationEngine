// Copyright (c) Microsoft Corporation. All rights reserved.
// 

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <windows.h>
#include <combaseapi.h>
#include <string>
#include <vector>
#include <tchar.h>
#include <regex>
#include <PathCch.h>
#include <atlsync.h>
#include <queue>

#include "../InstrumentationEngine.Lib/tstring.h"
#include "../InstrumentationEngine.Lib/CriticalSectionHolder.h"
#include "../InstrumentationEngine.Lib/InitOnce.h"
#include "../InstrumentationEngine.Lib/Singleton.h"
#include "../InstrumentationEngine.Lib/Macros.h"
#include "../InstrumentationEngine.Lib/EventLoggingBase.h"
#include "../ExtensionsHostLib/SafeFindFileHandle.h"