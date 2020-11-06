// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// RawProfilerHook.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "RawProfilerHook_i.h"
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

#pragma warning( push )
#pragma warning( disable: 28301 ) // No annotations for first declaration of 'DllRegisterServer'.

// DllRegisterServer - Adds entries to the system registry.
__control_entrypoint(DllExport)
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
    return hr;
}

// DllUnregisterServer - Removes entries from the system registry.
__control_entrypoint(DllExport)
STDAPI DllUnregisterServer(void)
{
    HRESULT hr = _AtlModule.DllUnregisterServer();
    return hr;
}

#pragma warning ( pop )

// DllInstall - Adds/Removes entries to the system registry per user per machine.
STDAPI DllInstall(BOOL bInstall, _In_opt_  LPCWSTR pszCmdLine)
{
    HRESULT hr = E_FAIL;
#pragma warning( push )
#pragma warning( disable: 25138 ) // static array 'const wchar_t szUserSwitch[5]' should be a local pointer.
    static const wchar_t szUserSwitch[] = L"user";
#pragma warning ( pop )

    if (pszCmdLine != NULL)
    {
        if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
        {
            ATL::AtlSetPerUserRegistration(true);
        }
    }

    if (bInstall)
    {
        hr = DllRegisterServer();
        if (FAILED(hr))
        {
            DllUnregisterServer();
        }
    }
    else
    {
        hr = DllUnregisterServer();
    }

    return hr;
}


