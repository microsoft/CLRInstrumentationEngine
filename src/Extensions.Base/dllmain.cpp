// Copyright (c) Microsoft Corporation. All rights reserved.
//

// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "HostExtension_i.h"
#include "dllmain.h"

#include "ExtensionsCommon/CrtDiagnostics.h"

CHostExtensionModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		Agent::Diagnostics::CCrtDiagnostics::Enable();
		break;
	}

	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved);
}
