// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "RawProfilerHookLoader.h"

using namespace ATL;
using namespace CommonLib;

namespace MicrosoftInstrumentationEngine
{
    typedef HRESULT (__stdcall *PfnDllGetClassObject)(
        _In_   REFCLSID rclsid,
        _In_   REFIID riid,
        _Out_  LPVOID *ppv);


    CRawProfilerHookLoader::CRawProfilerHookLoader() noexcept
    {
    }

    _Check_return_ HRESULT CRawProfilerHookLoader::LoadRawProfilerHookComponentFrom(
        _In_ const std::wstring& strModulePath,
        _In_ const GUID& clsidRawProfilerHook,
        _Out_ CComPtr<IUnknown>& spRawProfilerHook,
        _Out_ CModuleHandle& hRawProfilerHookModule)
    {
        IfFalseRet(strModulePath.size() >= 0, E_INVALIDARG);

        HRESULT hr = S_OK;

        CLogging::LogMessage(_T("Looking up RawProfilerHook component module '%s'"), strModulePath);

        // trying to obtain handle if a module is already loaded
        // call of GetModuleHandleEx with default (0) flags !increments! module ref count
        HMODULE hModule = 0;
        if (FALSE == ::GetModuleHandleEx(0, strModulePath.c_str(), &hModule))
        {
            CLogging::LogMessage(_T("RawProfilerHook component module not found in the process, attempting to load"));

            hModule = ::LoadLibrary(strModulePath.c_str());
            IfFalseRet(nullptr != hModule, HRESULT_FROM_WIN32(::GetLastError()));

            CLogging::LogMessage(_T("RawProfilerHook component module load complete"));
        }
        else
        {
            CLogging::LogMessage(_T("RawProfilerHook component module already loaded"));
        }

        hRawProfilerHookModule.Reset(hModule);

        auto pfnDllGetClassObject = reinterpret_cast<PfnDllGetClassObject>(
            ::GetProcAddress(
                hRawProfilerHookModule,
                "DllGetClassObject"));
        IfFalseRet(nullptr != pfnDllGetClassObject, HRESULT_FROM_WIN32(::GetLastError()));

        ATL::CComPtr<IClassFactory> spClassFactory;
        IfFailRet(
            pfnDllGetClassObject(
                clsidRawProfilerHook,
                IID_IClassFactory,
                reinterpret_cast<LPVOID*>(&spClassFactory.p)));

        IfFailRet(spClassFactory->CreateInstance(
            nullptr,
            IID_IUnknown,
            reinterpret_cast<LPVOID*>(&spRawProfilerHook.p)));

        return S_OK;
    }
}