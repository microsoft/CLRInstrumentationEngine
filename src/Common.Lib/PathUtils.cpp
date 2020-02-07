// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PathUtils.h"

#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy, wmemcpy can be used
#include <sstream>
#pragma warning(pop)

namespace CommonLib
{
	bool PathUtils::Exists(const std::wstring& strPath)
	{
		return ::GetFileAttributes(strPath.c_str()) != INVALID_FILE_ATTRIBUTES;
	}

	std::wstring PathUtils::GetDirFromPath(LPCWSTR path)
	{
		wchar_t drive[_MAX_DRIVE + 1] = { 0 };
		wchar_t dir[_MAX_DIR + 1] = { 0 };

		// parse app path and get app name
		_wsplitpath_s(path, drive, _countof(drive), dir, _countof(dir), 0, 0, 0, 0);

		std::wstring dirName(drive);
		dirName += dir;

		return dirName;
	}

	std::wstring PathUtils::Combine(const std::wstring& strPath1, const std::wstring& strPath2)
	{
		std::wostringstream strFormat;

		strFormat << strPath1;
		if (strPath1.back() != L'\\') { strFormat << L'\\'; }
		strFormat << strPath2;

		return strFormat.str();
	}
}