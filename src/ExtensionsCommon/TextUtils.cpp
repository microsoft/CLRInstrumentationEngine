// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "TextUtils.h"

namespace Agent
{
namespace Text
{
namespace TextUtils
{
	std::wstring g_strEmpty(L"");

	bool IsEmpty(const std::wstring& strInput)
	{
		return 0 == strInput.size() || strInput == g_strEmpty;
	}

	const std::wstring& GetEmpty()
	{
		return g_strEmpty;
	}
}
}
}