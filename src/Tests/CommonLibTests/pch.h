// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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
#else 
#include <windows.h>
#include <atlbase.h>
#include <atlcom.h>
#endif

#include "gtest/gtest.h"
#include "Common.Lib/refcount.h"

#if defined(PLATFORM_UNIX)
#define _WT(X) u ## X
#define _ST(X) u8 ## X
#else
#define _WT(X) L ## X
#define _ST(X) L ## X
#endif

#define ASSERT_OK(X) ASSERT_EQ(S_OK, (X))
#define EXPECT_OK(X) EXPECT_EQ(S_OK, (X))
#define EXPECT_FAIL(X) EXPECT_TRUE(FAILED(X))
#define EXPECT_NULL(X) EXPECT_EQ(nullptr, (X))
#define EXPECT_NOT_NULL(X) EXPECT_NE(nullptr, (X))
