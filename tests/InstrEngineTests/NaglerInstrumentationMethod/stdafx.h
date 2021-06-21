// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlstr.h>
#include <atlcoll.h>

#include <corhdr.h>
#include <cor.h>
#include <clrprofiler.h>

#include "InstrumentationEngine.h"

#include <msxml6.h>

#ifndef IfFailRet
#define IfFailRet(EXPR) \
do { if (FAILED(hr = (EXPR))) { ATLASSERT(!L"IfFailRet(" L#EXPR L") failed"); return hr; } } while (false)
#endif

#include <vector>
#include <memory>
#include <unordered_map>
#include <Shlwapi.h>
//#include <Pathcch.h> // For Windows 7 support, we cannot use PathCch functions.

using namespace std;
using namespace ATL;

#ifndef IfFailRet
#define IfFailRet(EXPR) \
    do { if (FAILED(hr = (EXPR))) { ATLASSERT(FALSE); return hr; } } while (false)
#endif

#include "../../../src/Common.Headers/tstring.h"