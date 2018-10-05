// Copyright (c) Microsoft Corporation. All rights reserved.
// 

// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "RawProfilerHook_i.h"
#include "dllmain.h"

CRawProfilerHookModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved);
}
