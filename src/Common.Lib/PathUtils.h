// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy, wmemcpy can be used
#include <string>
#include <sstream>
#pragma warning(pop)
namespace CommonLib
{
    class PathUtils
    {
    public:
        // <summary> Checks whether or not path exists </summary>
        // <param name="path"> path (absolute/relative) </param>
        // <returns> true if path exists; otherwiase - false </returns>
        static bool Exists(const std::wstring& strPath);

        // <summary> Generates directory name from full path </summary>
        // <param name="path"> full path </param>
        // <returns> generated directory name </returns>
        static std::wstring GetDirFromPath(LPCWSTR path);

        static std::wstring Combine(const std::wstring& strPath1, const std::wstring& strPath2);
    };
}