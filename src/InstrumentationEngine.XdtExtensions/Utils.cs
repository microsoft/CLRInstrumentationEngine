// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Microsoft.InstrumentationEngine.XdtExtensions
{
    internal static class Utils
    {
        public static string DecodeUnicodeChars(string encodedStr)
        {
            return Regex.Replace(encodedStr, @"0x\d+", (m) =>
            {
                return Convert.ToChar(Convert.ToInt32(m.Value, 16)).ToString();
            });
        }
    }
}
