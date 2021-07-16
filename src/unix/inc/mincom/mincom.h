// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// mincom.h : include file for defining macros necessary to compile
// code that use Microsoft convensions, without needing to compile and
// link the entire platform abstraction layer.
//

#pragma once

#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#define interface struct


#ifndef _DECLSPEC_DEFINED_
#define _DECLSPEC_DEFINED_

#if  defined(_MSC_VER)
#define DECLSPEC_NOVTABLE   __declspec(novtable)
#define DECLSPEC_IMPORT     __declspec(dllimport)
#define DECLSPEC_SELECTANY  __declspec(selectany)
#elif defined(__GNUC__)
#define DECLSPEC_NOVTABLE
#define DECLSPEC_IMPORT     
#define DECLSPEC_SELECTANY  __attribute__((weak))
#else
#define DECLSPEC_NOVTABLE
#define DECLSPEC_IMPORT
#define DECLSPEC_SELECTANY
#endif

#endif // !_DECLSPEC_DEFINED_

#ifndef COM_NO_WINDOWS_H
#define COM_NO_WINDOWS_H
#endif


#define PURE                    = 0
#define THIS_
#define THIS                void


#include "callconv.h"

// from the dotnet runtime Platform Abstraction Layer. We could consider
// duplicating.
#include "pal_mstypes.h"


#include "no_sal.h"
#include "no_sal2.h"

#include "hresults.h"

#include "minunknwn.h"

#define _T(X) u ## X