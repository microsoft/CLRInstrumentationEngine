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
#include "no_sal.h"
#include "mincom/mincom.h"
#include "mincom/ccomptrs.h"
#include "mincor/mincor.h"
#include "wcrt.h"
#define COM_NO_WINDOWS_H
#define __RPC_H__ // skip inclusion of rpc.h
#define __PALRT_H__ // skip PAL inclusion
typedef void* PRPC_MESSAGE;
typedef void* RPC_IF_HANDLE;
#define __ATL_MIN_COM__
#define __STDC_WANT_LIB_EXT1__ 1 // for memcpy_s
#else 

#include "targetver.h"
#include "resource.h"

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlstr.h>
#include <atlcoll.h>

using namespace ATL;
#endif


#include <corhdr.h>
//#include <cor.h>
#include <clrprofiler.h>

#include "InstrumentationEngine.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <memory.h>

using namespace std;

#include "../../../src/Common.Headers/tstring.h"
#include "../../../src/Common.Lib/systemstring.h"
#include "../../../src/Common.Lib/refcount.h"
#include "../../../src/Common.Lib/ImplQueryInterface.h"
#include "../../../src/Common.Lib/XmlDocWrapper.h"
#include "assertions.h"

using namespace CommonLib;

// for exporting __stdcall/__fastcall methods to a known name. ARGSIZE is in bytes.
#ifndef PLATFORM_UNIX
#ifdef _WIN64
#define DLLEXPORT(METHOD, ARGSIZE) __pragma(comment(linker, "/EXPORT:" #METHOD ",PRIVATE")) METHOD
#else
#define DLLEXPORT(METHOD, ARGSIZE) __pragma(comment(linker, "/EXPORT:" #METHOD "=_" #METHOD "@" #ARGSIZE ",PRIVATE")) METHOD
#endif
#else
#define DLLEXPORT(METHOD, ARGSIZE) __attribute__((visibility("default"))) METHOD
#endif