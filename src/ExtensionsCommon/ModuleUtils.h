// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Agent
{
namespace Io
{
namespace ModuleUtils
{
	HINSTANCE GetHostModule();
	std::wstring GetModuleDirectory(HINSTANCE hInstance);
}
}
}