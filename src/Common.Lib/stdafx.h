// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently.
//
// NOTE: Do note include this header in any other header files.
// The types declared in mincom will conflict with the types 
// that are declared by the CLR in libraries that use the PAL.
// Instead, all headers in this library should stand-alone as
// much as possible. Use forward-declarations if needed.

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers


#ifdef PLATFORM_UNIX
// NOTE: Avoid depending on the Core CLR pal in
// this library. We would like to be able to use
// it in our test dlls, without needing to
// link to and initialize the PAL.

#include "mincom/mincom.h"
#include "mincom/ccomptrs.h"
#define __ATL_MIN_COM__

#include <libxml/parser.h>
#include <libxml/tree.h>
#else 
#include <windows.h>
#include <atlbase.h>
#include <atlcom.h>

#include <msxml6.h>
#endif

#include "../Common.Headers/Macros.h"
#include "refcount.h"

#include "../Common.Headers/banned.h"

