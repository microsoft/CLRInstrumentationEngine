// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include <string>

#ifdef PLATFORM_UNIX
typedef std::basic_string<WCHAR, std::char_traits<WCHAR>, std::allocator<WCHAR>> tstring;
typedef std::basic_stringstream<WCHAR> tstringstream;
#else
typedef std::wstring tstring;
typedef std::wstringstream tstringstream;
#endif
typedef std::basic_ofstream<WCHAR, std::char_traits<WCHAR> > tofstream;