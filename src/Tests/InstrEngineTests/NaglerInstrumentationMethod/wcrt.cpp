#include "stdafx.h"
#include "wcrt.h"
#include <string>

using namespace CommonLib;

int __cdecl wcscmp(const WCHAR* string1, const WCHAR* string2)
{
    return wcsncmp(string1, string2, (size_t)0x7fffffff);
}

int __cdecl wcsncmp(const WCHAR* string1, const WCHAR* string2, size_t count)
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

INT32 __cdecl _wtoi(const WCHAR* s)
{
    std::string utf8;
    HRESULT hr = SystemString::Convert(s, utf8);
    if (hr < 0)
    {
        return 0;
    }

    return std::stoi(utf8);

}

INT64 __cdecl _wtoi64(const WCHAR* s)
{
    std::string utf8;
    HRESULT hr = SystemString::Convert(s, utf8);
    if (hr < 0)
    {
        return 0;
    }

    return std::stoll(utf8);
}