// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#define USE_DEPRECATED_CLR_API_WITHOUT_WARNING //  global SN APIs are used only as 2.0 fallback
#include "StrongName.h"
#undef USE_DEPRECATED_CLR_API_WITHOUT_WARNING

#include <mscoree.h>
#pragma warning( push )
#pragma warning( disable: 28740 ) // Unannotated unsigned buffer
#include <LegacyActivationShim.h>
#pragma warning( pop )

__encoded_pointer void* g_StrongNameTokenFromAssemblyPtr = EncodePointer(0);
__encoded_pointer void* g_StrongNameTokenFromPublicKeyPtr = EncodePointer(0);
__encoded_pointer void* g_StrongNameErrorInfoPtr = EncodePointer(0);
__encoded_pointer void* g_StrongNameFreeBufferPtr = EncodePointer(0);

using namespace LegacyActivationShim;
using namespace ATL;

HRESULT GetMetaHost(CComPtr<ICLRMetaHost>& host)
{
    // Use the utility to get the host
    // This API does *not* AddRef the returned interface
    ICLRMetaHost* hostNoAddref = 0;
    HRESULT hr = Util::GetCLRMetaHost(&hostNoAddref);
    if (FAILED(hr))
    {
        return hr;
    }

    if (!hostNoAddref)
    {
        return E_FAIL;
    }

    host = hostNoAddref;
    return S_OK;
}

LPVOID GetProcAddress(ICLRMetaHost* host, LPCSTR exportedFunction)
{
    // Enumerate the loaded runtimes to find one that supports the export we're looking for
    CComPtr<IEnumUnknown> enumerator;
    if (FAILED(host->EnumerateLoadedRuntimes(GetCurrentProcess(), &enumerator)))
    {
        return 0;
    }

    while (true)
    {
        // Get the next runtime that's loaded
        CComPtr<IUnknown> runtimeUnk;
        ULONG fetched = 0;
        if (enumerator->Next(1, &runtimeUnk, &fetched) != S_OK || fetched != 1)
        {
            break;
        }

        CComPtr<ICLRRuntimeInfo> runtime;
        runtime = runtimeUnk;
        if (runtime)
        {
            // If the runtime exports what we're looking for, we're done
            LPVOID address;
            if (SUCCEEDED(runtime->GetProcAddress(exportedFunction, &address)))
            {
                return address;
            }
        }
    }
    return 0;
}

template <typename T>
T GetFunctionPtr(void*& encodedPtr, LPCSTR exportedFunction, T defaultPtr)
{
    T ptr = reinterpret_cast<T>(DecodePointer(encodedPtr));
    if (!ptr)
    {
        // Check for a 4.0 CLR
        CComPtr<ICLRMetaHost> host;
        if (SUCCEEDED(GetMetaHost(host)))
        {
            // Get the export
            ptr = reinterpret_cast<T>(GetProcAddress(host, exportedFunction));
        }

        if (!ptr)
        {
            // Not found, use the default given
            ptr = defaultPtr;
        }

        encodedPtr = EncodePointer(ptr);
    }
    return ptr;
}

BOOLEAN (__stdcall *GetStrongNameTokenFromAssemblyPtr())(LPCWSTR, BYTE**, ULONG*)
{
    return GetFunctionPtr<BOOLEAN (__stdcall *)(LPCWSTR, BYTE**, ULONG*)>(g_StrongNameTokenFromAssemblyPtr, "StrongNameTokenFromAssembly", ::StrongNameTokenFromAssembly);
}

BOOLEAN (__stdcall *GetStrongNameTokenFromPublicKeyPtr())(BYTE*, ULONG, BYTE**, ULONG*)
{
    return GetFunctionPtr<BOOLEAN (__stdcall *)(BYTE*, ULONG, BYTE**, ULONG*)>(g_StrongNameTokenFromPublicKeyPtr, "StrongNameTokenFromPublicKey", ::StrongNameTokenFromPublicKey);
}

DWORD(__stdcall *GetStrongNameErrorInfoPtr())()
{
    return GetFunctionPtr<DWORD (__stdcall *)()>(g_StrongNameErrorInfoPtr, "StrongNameErrorInfo", ::StrongNameErrorInfo);
}

VOID (__stdcall *GetStrongNameFreeBufferPtr())(BYTE*)
{
    return GetFunctionPtr<VOID (__stdcall *)(BYTE*)>(g_StrongNameFreeBufferPtr, "StrongNameFreeBuffer", ::StrongNameFreeBuffer);
}

