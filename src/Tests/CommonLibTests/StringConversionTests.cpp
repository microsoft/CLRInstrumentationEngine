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

TEST(StringConversionTests, ConvertsNullEmptyAndAscii) {
    tstring wide = _WS("unchanged");
    EXPECT_EQ(E_INVALIDARG, SystemString::Convert(static_cast<const CHAR*>(nullptr), wide));
    EXPECT_TRUE(wide.empty());

    string utf8 = "unchanged";
    EXPECT_EQ(E_INVALIDARG, SystemString::Convert(static_cast<const WCHAR*>(nullptr), utf8));
    EXPECT_TRUE(utf8.empty());

    wide = _WS("unchanged");
    ASSERT_OK(SystemString::Convert("", wide));
    EXPECT_TRUE(wide.empty());
    EXPECT_EQ(static_cast<WCHAR>(0), wide.c_str()[wide.size()]);

    utf8 = "unchanged";
    ASSERT_OK(SystemString::Convert(_WS(""), utf8));
    EXPECT_TRUE(utf8.empty());
    EXPECT_EQ('\0', utf8.c_str()[utf8.size()]);

    ASSERT_OK(SystemString::Convert("CLRIE", wide));
    EXPECT_EQ(_WS("CLRIE"), wide);
    EXPECT_EQ(static_cast<WCHAR>(0), wide.c_str()[wide.size()]);

    ASSERT_OK(SystemString::Convert(_WS("CLRIE"), utf8));
    EXPECT_EQ("CLRIE", utf8);
    EXPECT_EQ('\0', utf8.c_str()[utf8.size()]);
}

TEST(StringConversionTests, ConvertsUnicodeBoundaries) {
    const char utf8[] = {
        static_cast<char>(0x01),
        static_cast<char>(0x7F),
        static_cast<char>(0xC2), static_cast<char>(0x80),
        static_cast<char>(0xDF), static_cast<char>(0xBF),
        static_cast<char>(0xE0), static_cast<char>(0xA0), static_cast<char>(0x80),
        static_cast<char>(0xED), static_cast<char>(0x9F), static_cast<char>(0xBF),
        static_cast<char>(0xEE), static_cast<char>(0x80), static_cast<char>(0x80),
        static_cast<char>(0xEF), static_cast<char>(0xBF), static_cast<char>(0xBF),
        static_cast<char>(0xF0), static_cast<char>(0x90), static_cast<char>(0x80), static_cast<char>(0x80),
        static_cast<char>(0xF4), static_cast<char>(0x8F), static_cast<char>(0xBF), static_cast<char>(0xBF),
        0
    };
    const WCHAR utf16[] = {
        static_cast<WCHAR>(0x0001),
        static_cast<WCHAR>(0x007F),
        static_cast<WCHAR>(0x0080),
        static_cast<WCHAR>(0x07FF),
        static_cast<WCHAR>(0x0800),
        static_cast<WCHAR>(0xD7FF),
        static_cast<WCHAR>(0xE000),
        static_cast<WCHAR>(0xFFFF),
        static_cast<WCHAR>(0xD800), static_cast<WCHAR>(0xDC00),
        static_cast<WCHAR>(0xDBFF), static_cast<WCHAR>(0xDFFF),
        static_cast<WCHAR>(0)
    };

    tstring wide;
    ASSERT_OK(SystemString::Convert(utf8, wide));
    EXPECT_EQ(tstring(utf16), wide);
    EXPECT_EQ(static_cast<WCHAR>(0), wide.c_str()[wide.size()]);

    string roundTrip;
    ASSERT_OK(SystemString::Convert(utf16, roundTrip));
    EXPECT_EQ(string(utf8), roundTrip);
    EXPECT_EQ('\0', roundTrip.c_str()[roundTrip.size()]);
}

TEST(StringConversionTests, StopsAtNullTerminator) {
    const char utf8[] = { 'A', '\0', static_cast<char>(0xFF), '\0' };
    tstring wide = _WS("unchanged");
    ASSERT_OK(SystemString::Convert(utf8, wide));
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

TEST(StringConversionTests, RejectsMalformedUtf8AndClearsDestination) {
    const char* malformed[] = {
        "\x80",
        "\xBF",
        "A\x80",
        "\xC2",
        "A\xC2",
        "\xE0\xA0",
        "\xF0\x90\x80",
        "\xE2\x28\xA1",
        "\xE2\x82\x41",
        "\xF0\x9F\x28\x80",
        "\xC0\x80",
        "\xC1\xBF",
        "\xE0\x80\x80",
        "\xF0\x80\x80\x80",
        "\xED\xA0\x80",
        "\xED\xBF\xBF",
        "\xF4\x90\x80\x80",
        "\xF5\x80\x80\x80",
        "\xF7\xBF\xBF\xBF",
        "\xF8\x88\x80\x80\x80",
        "\xFF"
    };

    for (size_t i = 0; i < sizeof(malformed) / sizeof(malformed[0]); ++i)
    {
        SCOPED_TRACE(i);
        tstring result = _WS("unchanged");
        EXPECT_EQ(E_INVALIDARG, SystemString::Convert(malformed[i], result));
        EXPECT_TRUE(result.empty());
    }
}

TEST(StringConversionTests, RejectsMalformedUtf16AndClearsDestination) {
    const WCHAR loneHigh[] = {
        static_cast<WCHAR>(0xD800),
        static_cast<WCHAR>(0)
    };
    const WCHAR loneLow[] = {
        static_cast<WCHAR>(0xDC00),
        static_cast<WCHAR>(0)
    };
    const WCHAR highThenBasic[] = {
        static_cast<WCHAR>(0xD800),
        static_cast<WCHAR>('A'),
        static_cast<WCHAR>(0)
    };
    const WCHAR highThenHigh[] = {
        static_cast<WCHAR>(0xD800),
        static_cast<WCHAR>(0xDBFF),
        static_cast<WCHAR>(0)
    };
    const WCHAR lowThenHigh[] = {
        static_cast<WCHAR>(0xDFFF),
        static_cast<WCHAR>(0xDBFF),
        static_cast<WCHAR>(0)
    };
    const WCHAR basicThenHigh[] = {
        static_cast<WCHAR>('A'),
        static_cast<WCHAR>(0xD800),
        static_cast<WCHAR>(0)
    };
    const WCHAR* malformed[] = {
        loneHigh,
        loneLow,
        highThenBasic,
        highThenHigh,
        lowThenHigh,
        basicThenHigh
    };

    for (size_t i = 0; i < sizeof(malformed) / sizeof(malformed[0]); ++i)
    {
        SCOPED_TRACE(i);
        string result = "unchanged";
        EXPECT_EQ(E_INVALIDARG, SystemString::Convert(malformed[i], result));
        EXPECT_TRUE(result.empty());
    }
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

    overLength[0] = static_cast<char>(0xFF);
    result = _WS("unchanged");
    EXPECT_EQ(E_BOUNDS, SystemString::Convert(overLength.data(), result));
    EXPECT_EQ(_WS("unchanged"), result);

    vector<char> unterminated(10000, 'A');
    result = _WS("unchanged");
    EXPECT_EQ(E_BOUNDS, SystemString::Convert(unterminated.data(), result));
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

    overLength[0] = static_cast<WCHAR>(0xDC00);
    result = "unchanged";
    EXPECT_EQ(E_BOUNDS, SystemString::Convert(overLength.data(), result));
    EXPECT_TRUE(result.empty());

    vector<WCHAR> unterminated(10000, static_cast<WCHAR>('A'));
    result = "unchanged";
    EXPECT_EQ(E_BOUNDS, SystemString::Convert(unterminated.data(), result));
    EXPECT_TRUE(result.empty());
}

#endif
