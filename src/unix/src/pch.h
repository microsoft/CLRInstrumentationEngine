// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#define MAX_LEADBYTES         12
#include "unix.h"

#include <cstdint>

#include "dbgmsg.h"
#include "_vsassert.h"

#define ATLASSERT(cond) VSASSERT(cond, "ATLASSERT FAILED")
#include <atlstr.h>

#define ASSERT(msg)  VSFAIL(msg)
// TODO:DEBUG
#define ASSERT(msg, args...)

#define PERF_ENTRY(x)
#define PERF_EXIT(x)

// String functions

#define MAX_LEADBYTES             12          // 5 ranges, 2 bytes ea., 0 term.
#define MAX_DEFAULTCHAR           2           // single or double byte

// Winnls.h

// Winnt.h



// Misc
#define PAL_free(x) free(x)
#define PAL_malloc(x) malloc(x)
#define PAL_realloc(m, size) realloc(m, size)
