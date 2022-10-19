#pragma once

#include <cstddef>
// char16_t equivalents of standard library functions.

int __cdecl wcscmp(const WCHAR* string1, const WCHAR* string2);
int __cdecl wcsncmp(const WCHAR* string1, const WCHAR* string2, size_t count);
INT32 __cdecl _wtoi(const WCHAR* s);
INT64 __cdecl _wtoi64(const WCHAR* s);
