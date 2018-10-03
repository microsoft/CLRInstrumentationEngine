// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// ported from coreclr\pal\inc\pal.h

#include "pch.h"

#if !defined(_TRUNCATE)
#define _TRUNCATE ((size_t)-1)
#endif

// todo: determine if supporting errno is necessary
// actual from pal.h:
/*
extern int* __cdecl _errno(void);
#define errno (*_errno())

#if !defined(ERANGE)
#define ERANGE          34
#endif
*/

