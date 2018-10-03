// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "PathUtils.h"

namespace Agent
{
namespace Io
{
namespace PathUtils
{
	bool Exists(const std::wstring& strPath)
	{
		return ::GetFileAttributes(strPath.c_str()) != INVALID_FILE_ATTRIBUTES;
	}

	std::wstring GetAbsolutePath(const std::wstring& strPath)
	{
		std::wstring strAbsolutePath(_MAX_PATH + 1, L'\0');

		LPWSTR* lppPart = nullptr;

		DWORD dwCount = ::GetFullPathName(
			strPath.c_str(),
			static_cast<DWORD>(strAbsolutePath.size()),
			&strAbsolutePath[0],
			lppPart);

		strAbsolutePath.resize(dwCount);

		return strAbsolutePath;
	}

	std::wstring GetDirFromPath(LPCWSTR path)
	{
		wchar_t drive[_MAX_DRIVE + 1] = { 0 };
		wchar_t dir[_MAX_DIR + 1] = { 0 };

		// parse app path and get app name
		_wsplitpath_s(path, drive, _countof(drive), dir, _countof(dir), 0, 0, 0, 0);

		std::wstring dirName(drive);
		dirName += dir;

		return dirName;
	}

	std::wstring GetFileNameFromPath(LPCWSTR path)
	{
		// parse app path and get app name
		wchar_t ext[_MAX_EXT + 1] = { 0 };
		wchar_t name[_MAX_FNAME + 1] = { 0 };

		_wsplitpath_s(path, 0, 0, 0, 0, name, _countof(name), ext, _countof(ext));

		std::wstring fileName(name);
		fileName += ext;

		return fileName;
	}

	std::wstring getFileNameFromPathWithoutExtension(LPCWSTR path)
	{
		// parse app path and get app name
		wchar_t name[_MAX_FNAME] = { 0 };

		_wsplitpath_s(path, 0, 0, 0, 0, name, _countof(name), 0, 0);

		return name;
	}

	std::wstring Combine(const std::wstring& strPath1, const std::wstring& strPath2)
	{
		std::wostringstream strFormat;

		strFormat << strPath1;
		if (strPath1.back() != L'\\') { strFormat << L'\\'; }
		strFormat << strPath2;

		return strFormat.str();
	}
}
}
}