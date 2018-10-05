// Copyright (c) Microsoft Corporation. All rights reserved.
// 

// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "ExtensionsHost_i.h"
#include "dllmain.h"

#include "ExtensionsCommon/CrtDiagnostics.h"

CExtensionsHostModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		Agent::Diagnostics::CCrtDiagnostics::Enable();
		break;
	}

	return _AtlModule.DllMain(dwReason, lpReserved);
}
