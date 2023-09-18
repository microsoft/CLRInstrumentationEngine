//
// (c) Copyright IBM Corp. 2021
// (c) Copyright Instana Inc. 2021
//
#include "ConversionUtil.h"
#include <iostream>
#include <stdio.h>
#include <cuchar>
#ifdef _WINDOWS
std::string ConvertString(WCHAR* wstr) { //WHCAR is char16_t
    std::wstring source(wstr);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
    std::string str = convert.to_bytes(source);

    return str;
}

std::wstring ConvertString(std::string source)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
    return convert.from_bytes(source);

}

#else

std::string ConvertString(WCHAR* wstr, const char* where) { //WHCAR is char16_t
    try
    {
        std::u16string source(wstr);
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        std::string str = convert.to_bytes(source);

        return str;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error converting things (WCHAR):" << e.what() << where << '\n';
    }
}


std::string ConvertString(const WCHAR* wstr) { //WHCAR is char16_t
    try
    {
        std::u16string source(wstr);
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        std::string str = convert.to_bytes(source);

        return str;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error converting things (const WCHAR):" << e.what() << wstr << '\n';
    }

}
std::u16string ConvertString(std::string source)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.from_bytes(source);
}

std::u16string StringtoU16(const std::string& str) {
    std::u16string wstr = u"";
    char16_t c16str[3] = u"\0";
    mbstate_t mbs;
    for (const auto& it : str) {
        memset(&mbs, 0, sizeof(mbs));//set shift state to the initial state
        memmove(c16str, u"\0\0\0", 3);
        mbrtoc16(c16str, &it, 3, &mbs);
        wstr.append(std::u16string(c16str));
    }//for
    return wstr;
}
#endif