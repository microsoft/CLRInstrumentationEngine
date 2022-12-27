//
// (c) Copyright IBM Corp. 2021
// (c) Copyright Instana Inc. 2021
//
#pragma once

#include <cor.h>
#include <locale>
#include <codecvt>
#include <string>

#ifdef _WINDOWS
std::string ConvertString(WCHAR* wstr);
std::wstring ConvertString(std::string source);
#else
std::string ConvertString(WCHAR* wstr, const char* where);
std::u16string ConvertString(std::wstring source);
std::u16string StringtoU16(const std::string &str);
#endif
