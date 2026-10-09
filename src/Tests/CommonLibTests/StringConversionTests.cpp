// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Common.Lib/systemstring.h"
#include <vector>

using namespace std;
using namespace CommonLib;
TEST(StringConversionTests, u8u16test) {
    const char* ss = u8"①Ⅻㄨㄩ 啊阿鼾齄丂丄狚狛狜狝﨨﨩ˊˋ˙– ⿻〇㐀㐁䶴䶵";
    tstring u16test = _WS("①Ⅻㄨㄩ 啊阿鼾齄丂丄狚狛狜狝﨨﨩ˊˋ˙– ⿻〇㐀㐁䶴䶵");
    tstring tresult;
    EXPECT_OK(SystemString::Convert(ss, tresult));
    EXPECT_EQ(u16test, tresult);
}

TEST(StringConversionTests, u16u8test) {
    const WCHAR* ss = _WS("①Ⅻㄨㄩ 啊阿鼾齄丂丄狚狛狜狝﨨﨩ˊˋ˙– ⿻〇㐀㐁䶴䶵");
    string u8test = u8"①Ⅻㄨㄩ 啊阿鼾齄丂丄狚狛狜狝﨨﨩ˊˋ˙– ⿻〇㐀㐁䶴䶵";
    string u8result;
    EXPECT_OK(SystemString::Convert(ss, u8result));
    EXPECT_EQ(u8test, u8result);
}

#ifdef PLATFORM_UNIX

TEST(StringConversionTests, PreservesNullEmptyAndTerminatorSemantics) {
    tstring wide = _WS("unchanged");
    EXPECT_EQ(E_INVALIDARG, SystemString::Convert(static_cast<const CHAR*>(nullptr), wide));
    EXPECT_TRUE(wide.empty());

    string utf8 = "unchanged";
    EXPECT_EQ(E_INVALIDARG, SystemString::Convert(static_cast<const WCHAR*>(nullptr), utf8));
    EXPECT_TRUE(utf8.empty());

    wide = _WS("unchanged");
    ASSERT_OK(SystemString::Convert("", wide));
    EXPECT_TRUE(wide.empty());

    utf8 = "unchanged";
    ASSERT_OK(SystemString::Convert(_WS(""), utf8));
    EXPECT_TRUE(utf8.empty());

    const char terminatedUtf8[] = { 'A', '\0', static_cast<char>(0xFF), '\0' };
    wide = _WS("unchanged");
    ASSERT_OK(SystemString::Convert(terminatedUtf8, wide));
    EXPECT_EQ(_WS("A"), wide);

    const WCHAR utf16[] = {
        static_cast<WCHAR>('A'),
        static_cast<WCHAR>(0),
        static_cast<WCHAR>(0xD800),
        static_cast<WCHAR>(0)
    };
    string converted = "unchanged";
    ASSERT_OK(SystemString::Convert(utf16, converted));
    EXPECT_EQ("A", converted);
}

TEST(StringConversionTests, ConvertsSupplementaryScalarWithUnixWchar) {
    const char utf8[] = {
        static_cast<char>(0xF0),
        static_cast<char>(0x9F),
        static_cast<char>(0x98),
        static_cast<char>(0x80),
        0
    };
    const WCHAR utf16[] = {
        static_cast<WCHAR>(0xD83D),
        static_cast<WCHAR>(0xDE00),
        static_cast<WCHAR>(0)
    };

    tstring wide;
    ASSERT_OK(SystemString::Convert(utf8, wide));
    EXPECT_EQ(tstring(utf16), wide);

    string roundTrip;
    ASSERT_OK(SystemString::Convert(utf16, roundTrip));
    EXPECT_EQ(string(utf8), roundTrip);
}

TEST(StringConversionTests, MapsRepresentativeUtf8ErrorsAndClearsDestination) {
    const char* malformed[] = {
        "\x80",
        "\xE2\x82",
        "\xED\xA0\x80",
    };

    for (size_t i = 0; i < sizeof(malformed) / sizeof(malformed[0]); ++i)
    {
        SCOPED_TRACE(i);
        tstring result = _WS("unchanged");
        EXPECT_EQ(E_INVALIDARG, SystemString::Convert(malformed[i], result));
        EXPECT_TRUE(result.empty());
    }
}

TEST(StringConversionTests, MapsMalformedUtf16AndClearsDestination) {
    const WCHAR highThenBasic[] = {
        static_cast<WCHAR>(0xD800),
        static_cast<WCHAR>('A'),
        static_cast<WCHAR>(0)
    };

    string result = "unchanged";
    EXPECT_EQ(E_INVALIDARG, SystemString::Convert(highThenBasic, result));
    EXPECT_TRUE(result.empty());
}

TEST(StringConversionTests, EnforcesUtf8LengthBoundaryWithoutChangingDestination) {
    vector<char> maximumLength(9999, 'A');
    maximumLength[9998] = '\0';

    tstring result = _WS("unchanged");
    ASSERT_OK(SystemString::Convert(maximumLength.data(), result));
    EXPECT_EQ(static_cast<size_t>(9998), result.length());

    vector<char> overLength(10000, 'A');
    overLength[9999] = '\0';
    result = _WS("unchanged");
    EXPECT_EQ(E_BOUNDS, SystemString::Convert(overLength.data(), result));
    EXPECT_EQ(_WS("unchanged"), result);
}

TEST(StringConversionTests, EnforcesUtf16LengthBoundaryAndClearsDestination) {
    vector<WCHAR> maximumLength(10000, static_cast<WCHAR>('A'));
    maximumLength[9999] = static_cast<WCHAR>(0);

    string result = "unchanged";
    ASSERT_OK(SystemString::Convert(maximumLength.data(), result));
    EXPECT_EQ(static_cast<size_t>(9999), result.length());

    vector<WCHAR> overLength(10001, static_cast<WCHAR>('A'));
    overLength[10000] = static_cast<WCHAR>(0);
    result = "unchanged";
    EXPECT_EQ(E_BOUNDS, SystemString::Convert(overLength.data(), result));
    EXPECT_TRUE(result.empty());
}

#endif
