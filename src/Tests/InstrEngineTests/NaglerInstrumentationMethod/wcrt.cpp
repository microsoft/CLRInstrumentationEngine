#include "widechar.h"

int __cdecl wcscmp(const char16_t* string1, const char16_t* string2)
{
    return wcsncmp(string1, string2, 0x7fffffff);
}

int __cdecl wcsncmp(const char16_t *string1, const char16_t *string2, size_t count)
{
    size_t i;
    int diff = 0;
    for (i = 0; i < count; ++i)
    {
        diff = string1[i] - string2[i];
        if ((diff != 0) || (string1[i] == 0))
        { 
            return diff;
        }
    }

    return diff;
}