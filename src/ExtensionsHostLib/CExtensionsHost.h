// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// CExtensionsHost.h : Declaration of the CExtensionsHost

#pragma once

#include "stdafx.h"

#ifndef PLATFORM_UNIX
#include "./Platform/CExtensionsHostWindows.h"
#else
#include "./Platform/CExtensionsHostUnix.h"
#endif
