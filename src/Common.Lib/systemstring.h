// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Provides interfaces for working with cross-process strings.

#pragma once

#include "../Common.Headers/tstring.h"

namespace CommonLib
{
#ifdef PLATFORM_UNIX
    using osstring = std::string;
#else
    using osstring = std::wstring;
#endif

    /// <summary>
    /// Class for converting between operating system strings and standard strings
    /// or wide-character (Windows) strings. Useful for converting strings between
    /// Linux and Windows.
    /// </summary>
    class SystemString
    {
    public:
        /// <summary>
        /// Converts a UTF16LE (Windows) string to a UTF8 (Linux) string
        /// </summary>
        /// <param name="converted">The converted string</param>
        /// <returns>S_OK for success. Error code otherwise.</returns>
        static HRESULT Convert(_In_z_ const WCHAR*, _Inout_ std::string& converted);

        /// <summary>
        /// Utility function to convert a UTF8 (Linux) string to a UTF16LE (Windows) string.
        /// </summary>
        /// <param name="lpzStr">The input string.</param>
        /// <param name="result">The output string.</param>
        /// <returns>S_OK for success, error otherwise.</returns>
        static HRESULT Convert(_In_z_ const CHAR* lpzStr, _Inout_ tstring& result); // lgtm[cpp/inconsistentsal] SAL annotations match definition in cpp file
    };

}


