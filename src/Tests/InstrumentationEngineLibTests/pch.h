// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// We use the deprecated experimental libraries until we can upgrade to c++17
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING



#ifdef PLATFORM_UNIX
#include "unix.h"
#include <ole.h>
#include <palrt.h>
#include <pal.h>
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
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

#include <gtest/gtest.h>

// __valid is used as a SAL annotation, but it is also
// used as type alias on some Linux systems. Undefine it
// before including headers that use the type alias.
#ifdef __valid
#define __valid__temp __valid
#undef __valid
#endif

#include <experimental/filesystem>

#ifdef __valid__temp
#define __valid __valid__temp
#undef __valid__temp
#endif

#include "cor.h"
#include "corprof.h"

#include "Common.Lib/systemstring.h"
#include "Common.Lib/refcount.h"


#if defined(PLATFORM_UNIX)
#define _WT(X) u ## X
#define _ST(X) u8 ## X
#else
#define _WT(X) L ## X
#define _ST(X) L ## X
#endif


using namespace std;

#define ASSERT_OK(X) ASSERT_EQ(S_OK, (X))
#define ASSERT_FAIL(X) (ASSERT_TRUE(FAILED(X)))
#define ASSERT_NULL(X) (ASSERT_EQ(nullptr, (X)))
#define ASSERT_NOT_NULL(X) (ASSERT_NE(nullptr, (X)))
