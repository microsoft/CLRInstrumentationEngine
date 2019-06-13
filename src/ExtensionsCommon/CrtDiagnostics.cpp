// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "CrtDiagnostics.h"

#include <crtdbg.h>

namespace Agent
{
namespace Diagnostics
{
	void CCrtDiagnostics::Enable()
	{
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

		auto crtDebugFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

		crtDebugFlags |= _CRTDBG_ALLOC_MEM_DF;	// track allocations and mark them
		crtDebugFlags |= _CRTDBG_LEAK_CHECK_DF; // dump leak report at shutdown

		_CrtSetDbgFlag(crtDebugFlags);
	}
}
}