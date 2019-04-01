// Copyright (c) Microsoft Corporation. All rights reserved.
//

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

#include "../InstrumentationEngine/refcount.h"
#include "../InstrumentationEngine/ImplQueryInterface.h"
#include "../InstrumentationEngine.Api/InstrumentationEngine.h"
#include "../InstrumentationEngine.Lib/Logging.h"

#endif