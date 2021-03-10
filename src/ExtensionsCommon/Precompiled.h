// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifdef PLATFORM_UNIX
#include "unix.h"
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOSERVICE
#define NOMCX
#define NOIME
#define NOSOUND
#define NOCOMM
#define NOKANJI
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

// ATL includes
#include <atlbase.h>
#include <atlcom.h>

#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy can be used

// stl misc
#include <limits>
#include <utility>
#include <algorithm>
#include <functional>
#include <tuple>

// stl containers
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

// stl streams formatting and IO
#include <iomanip>
#include <sstream>

// stl memory management (smart pointers)
#include <memory>

// stl threading

#include <atomic>

#include <mutex>
#include <thread>
#include <future>

#pragma warning(pop)

#include "../Common.Headers/banned.h"

// Microsoft Instrumentation Engine encludes
#include "InstrumentationEngineDefs.h"
#include "InstrumentationEngineInfraPointerDefs.h"
#include "InstrumentationEnginePointerDefs.h"

// common Agent headers
#include "AgentValidation.h"
#include "AgentTrace.h"

// common Diagnostics headers
#include "Exception.h"
#include "FormatMessage.h"

// misc Agent service headers
#include "ProfilerHostServices.h"
