// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifdef PLATFORM_UNIX
#define FILE PAL_FILE
#endif

// Deleter for FILE
namespace MicrosoftInstrumentationEngine
{
    class FILEDeleter
    {
    public:
        void operator()(FILE* pFile)
        {
            fclose(pFile);
        }
    };
}