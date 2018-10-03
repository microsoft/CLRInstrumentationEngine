// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "refcount.h"
#include "CExtensionsHost.h"

namespace ExtensionsHostCrossPlat
{

    class CLinuxClassFactory : public IClassFactory, public CModuleRefCount
    {
    private:

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CLinuxClassFactory);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CLinuxClassFactory);
        STDMETHOD(STDMETHODCALLTYPE) QueryInterface(REFIID riid, PVOID *ppvObject)
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
            )
        {
            HRESULT hr = S_OK;

            CComPtr<CExtensionHost> pExtensionHost;
            pExtensionHost.Attach(new CExtensionHost());

            IfFailRet(pExtensionHost->QueryInterface(riid, ppvObject));

            return S_OK;
        }

        STDMETHOD(LockServer)(
            _In_ BOOL fLock
            )
        {
            return S_OK;
        }
    };
}

extern "C" HRESULT __attribute__((visibility("default"))) DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR* ppv)
{
    CComPtr<IClassFactory> pClassFactory;
    pClassFactory.Attach(new ExtensionsHostCrossPlat::CLinuxClassFactory);

    if (pClassFactory != nullptr)
    {
        return pClassFactory->QueryInterface(riid, ppv);
    }

    return E_NOINTERFACE;
}
