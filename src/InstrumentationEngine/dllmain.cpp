// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "ProfilerManager.h"
#include "LoggingWrapper.h"

#ifndef PLATFORM_UNIX

#include "AtlModule.h"

extern MicrosoftInstrumentationEngine::CCustomAtlModule _AtlModule;

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
#ifdef DEBUG
        // Uncomment this code to track down a memory leak based on allocation count
        // The leak allocation count is the number in the {} when the leak is dumped
        // _CrtSetBreakAlloc(/* LEAK ALLOCATION COUNT HERE */);

        _CrtSetDbgFlag(
            _CRTDBG_ALLOC_MEM_DF | // track allocations and mark them
            _CRTDBG_LEAK_CHECK_DF // dump leak report at shutdown
        );
#endif
        INITIALIZE_REF_RECORDER(MicrosoftInstrumentationEngine::CRefCount::EnableRecorder);
        break;

    case DLL_PROCESS_DETACH:
        DUMP_REFCOUNT;
        TERMINATE_REF_RECORDER;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
        break;
    }

    return _AtlModule.DllMain(dwReason, lpReserved);
}
#endif // !PLATFORM_UNIX

class CCrossPlatClassFactory : public IClassFactory, public CModuleRefCount
{
private:

public:
    DEFINE_DELEGATED_REFCOUNT_ADDREF(CCrossPlatClassFactory);
    DEFINE_DELEGATED_REFCOUNT_RELEASE(CCrossPlatClassFactory);
    STDMETHOD(STDMETHODCALLTYPE) QueryInterface(REFIID riid, PVOID *ppvObject) override
    {
        HRESULT hr = E_NOINTERFACE;

        hr = ImplQueryInterface(
            static_cast<IClassFactory*>(this),
            riid, ppvObject
        );

        return hr;
    }

    // IClassFactory Methods
public:
    STDMETHOD(CreateInstance)(
        _In_ IUnknown *pUnkOuter,
        _In_   REFIID   riid,
        _Out_ void **ppvObject
        ) override
    {
        HRESULT hr = S_OK;

        CComObject<CProfilerManager>* pProfilerManager;
        IfFailRet(CComObject<CProfilerManager>::CreateInstance(&pProfilerManager));

        IfFailRet(pProfilerManager->QueryInterface(riid, ppvObject));

        return S_OK;
    }

    STDMETHOD(LockServer)(
        _In_ BOOL fLock
        ) override
    {
        return S_OK;
    }
};

#ifndef PLATFORM_UNIX
__control_entrypoint(DllExport)
STDAPI DLLEXPORT(DllCanUnloadNow, 0)(void)
{
    return _AtlModule.DllCanUnloadNow();
}
#endif

_Check_return_
STDAPI DLLEXPORT(DllGetClassObject, 12)(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR* ppv)
{
    CComPtr<IClassFactory> pClassFactory;
    pClassFactory.Attach(new CCrossPlatClassFactory);

    if (pClassFactory != nullptr)
    {
        return pClassFactory->QueryInterface(riid, ppv);
    }

    return E_NOINTERFACE;
}

#ifndef PLATFORM_UNIX
__control_entrypoint(DllExport)
STDAPI DLLEXPORT(DllRegisterServer, 0)(void)
{
    return _AtlModule.DllRegisterServer(false);
}

__control_entrypoint(DllExport)
STDAPI DLLEXPORT(DllUnregisterServer, 0)(void)
{
    return _AtlModule.DllRegisterServer(false);
}
#endif

STDAPI DLLEXPORT(GetInstrumentationEngineLogger, 4)(_Outptr_ IProfilerManagerLogging** ppLogging)
{
    if (nullptr == ppLogging)
    {
        return E_POINTER;
    }

    CComPtr<CLoggingWrapper> pLogging;
    pLogging.Attach(new (std::nothrow) CLoggingWrapper());
    if (nullptr == pLogging)
    {
        return E_OUTOFMEMORY;
    }

    HRESULT hr = S_OK;
    IfFailRetNoLog(pLogging->Initialize());

    *ppLogging = pLogging.Detach();
    return S_OK;
}
