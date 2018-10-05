// Copyright (c) Microsoft Corporation. All rights reserved.
// 

// HostExtension.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "HostExtension_i.h"
#include "dllmain.h"


using namespace ATL;

// Used to determine whether the DLL can be unloaded by OLE.
__control_entrypoint(DllExport)
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type.
_Check_return_
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID* ppv)
{
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

