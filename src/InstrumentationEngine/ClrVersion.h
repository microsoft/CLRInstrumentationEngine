// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

namespace MicrosoftInstrumentationEngine
{
    enum ClrVersion : WORD // higher byte: major, lower byte: minor
    {
        ClrVersion_Unknown  = 0,
        ClrVersion_2        = 0x0200, // v2.0
        ClrVersion_4        = 0x0400, // v4.0
        ClrVersion_4_5      = 0x0405, // v4.5
    };
}
