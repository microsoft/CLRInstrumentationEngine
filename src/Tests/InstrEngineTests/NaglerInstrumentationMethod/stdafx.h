// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#ifdef PLATFORM_UNIX
// NOTE: Avoid depending on the Core CLR pal in
// this library. We would like to be able to use
// it in our test dlls, without needing to
// link to and initialize the PAL.

#include "mincom/mincom.h"
#include "mincom/ccomptrs.h"
#define __ATL_MIN_COM__
#else 

#include "targetver.h"
#include "resource.h"

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlstr.h>
#include <atlcoll.h>
#endif


#include <corhdr.h>
#include <cor.h>
#include <clrprofiler.h>

#include "InstrumentationEngine.h"
using namespace ATL;

inline void AssertFailed(_In_ const WCHAR* failure);

#include <vector>
#include <memory>
#include <unordered_map>

using namespace std;

#include "../../../src/Common.Headers/tstring.h"
#include "../../../src/Common.Lib/refcount.h"
#include "../../../src/Common.Lib/ImplQueryInterface.h"
#include "../../../src/Common.Lib/XmlDocWrapper.h"

#ifndef IfFailRet
#define IfFailRet(EXPR) \
do { if (FAILED(hr = (EXPR))) { AssertFailed(_T("IfFailRet(") _T(#EXPR) _T(") failed")); return hr; } } while (false)
#endif

using namespace CommonLib;