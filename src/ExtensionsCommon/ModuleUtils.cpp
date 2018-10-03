// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "ModuleUtils.h"
#include "PathUtils.h"

namespace Agent
{
namespace Io
{
namespace ModuleUtils
{

	HMODULE g_hModuleMarker = nullptr;

	/// <summary>
	/// Gets handle of the host module
	/// </summary>
	HINSTANCE GetHostModule()
	{
		if (nullptr == g_hModuleMarker)
		{
			::GetModuleHandleEx(
				GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
				| GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
				reinterpret_cast<LPCWSTR>(&g_hModuleMarker),
				&g_hModuleMarker);
		}

		return g_hModuleMarker;
	}

	std::wstring GetModuleDirectory(HINSTANCE hInstance)
	{
		std::wstring result(L"");
		wchar_t szRelativePath[_MAX_PATH + 1] = { 0 };

		if (0 != ::GetModuleFileName(hInstance, szRelativePath, _countof(szRelativePath)))
		{
			wchar_t szAbsolutePath[_MAX_PATH + 1] = { 0 };

			LPWSTR* lppPart = nullptr;
			::GetFullPathName(szRelativePath, _countof(szAbsolutePath), szAbsolutePath, lppPart);

			result = Agent::Io::PathUtils::GetDirFromPath(szAbsolutePath);
		}

		return result;
	}
}
}
}