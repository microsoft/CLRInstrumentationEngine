// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

namespace Agent
{
namespace Io
{
namespace PathUtils
{
	// <summary> Checks whether or not path exists </summary>
	// <param name="path"> path (absolute/relative) </param>
	// <returns> true if path exists; otherwiase - false </returns>
	bool Exists(const std::wstring& strPath);

	// <summary> Gets absolute path from any path provided </summary>
	// <param name="path"> path (absolute/relative) </param>
	// <returns> string that represents path </returns>
	std::wstring GetAbsolutePath(const std::wstring& strPath);

	// <summary> Generates directory name from full path </summary>
	// <param name="path"> full path </param>
	// <returns> generated directory name </returns>
	std::wstring GetDirFromPath(wchar_t const * szPath);

	// <summary> Generates file name with extension from full path </summary>
	// <param name="path"> full path </param>
	// <returns> generated file name </returns>
	std::wstring GetFileNameFromPath(wchar_t const * szPath);

	// <summary> Generates file name without extension from full path </summary>
	// <param name="path"> full path </param>
	// <returns> generated file name </returns>
	std::wstring GetFileNameFromPathWithoutExtension(wchar_t const * szPath);

	std::wstring Combine(const std::wstring& strPath1, const std::wstring& strPath2);
}
}
}