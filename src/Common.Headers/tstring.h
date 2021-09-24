// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy can be used

#ifndef PLATFORM_UNIX
#include <Windows.h>
#endif

#include <string>
#pragma warning(pop)

#ifdef PLATFORM_UNIX
typedef std::basic_string<WCHAR, std::char_traits<WCHAR>, std::allocator<WCHAR>> tstring;
#else
typedef std::wstring tstring;
#endif
typedef std::basic_ofstream<WCHAR, std::char_traits<WCHAR> > tofstream;
