// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Common.Lib/systemstring.h"

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

