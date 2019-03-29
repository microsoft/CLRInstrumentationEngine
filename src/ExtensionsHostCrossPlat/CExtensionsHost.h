// Copyright (c) Microsoft Corporation. All rights reserved.
//

// CExtensionsHost.h : Declaration of the CExtensionsHost

#pragma once

// {CA487940-57D2-10BF-11B2-A3AD5A13CBC0}
const GUID CLSID_ExtensionHost =
{ 0xCA487940, 0x57D2, 0x10BF,{ 0x11, 0xB2, 0xA3, 0xAD, 0x5A, 0x13, 0xCB, 0xC0 } };

namespace ExtensionsHostCrossPlat
{

    // The existing ExtensionsHost had too much non-portable code to port to Linux.
    // In order to avoid destabilizing appinsights through a rewrite, this class
    // has been created to provide basic instrumentation engine host support for
    // production breakpoints.
    class CExtensionsHost :
        public IProfilerManagerHost,
        public CModuleRefCount
    {
    public:
        CExtensionsHost()
        {
#ifdef PLATFORM_UNIX
            PAL_Initialize(0, NULL);
#endif
        }

        CExtensionsHost(const CExtensionsHost&) = delete;

        DEFINE_DELEGATED_REFCOUNT_ADDREF(CExtensionsHost)
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CExtensionsHost)

        STDMETHOD(STDMETHODCALLTYPE) QueryInterface(REFIID riid, PVOID* ppvObject) override
        {
            HRESULT hr = E_NOINTERFACE;

            hr = ImplQueryInterface(
                static_cast<IProfilerManagerHost*>(this),
                riid,
                ppvObject
            );

            return hr;
        }

        // IProfilerManagerHost methods
    public:
        STDMETHOD(Initialize)(_In_ IProfilerManager* pProfilerManager) override;

    private:
        static void SetLoggingFlags(_In_ IProfilerManagerLogging* pLogger);
    };

}