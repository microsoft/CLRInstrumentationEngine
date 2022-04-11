#pragma once

#include <cstddef>
// char16_t equivalents of standard library functions.

int __cdecl wcscmp(const char16_t* string1, const char16_t* string2);
int __cdecl wcsncmp(const char16_t *string1, const char16_t *string2, size_t count);