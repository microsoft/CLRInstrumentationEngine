// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef PLATFORM_UNIX

#include "../ExtensionsCommon/Precompiled.h"

#else

#include "unix.h"
#include <cor.h>

using namespace std;

#include "../InstrumentationEngine.Api/InstrumentationEngine.h"
#include "../InstrumentationEngine.Lib/ImplQueryInterface.h"
#include "../InstrumentationEngine.Lib/Logging.h"
#include "../InstrumentationEngine.Lib/refcount.h"

#endif