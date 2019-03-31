#pragma once
#include "stdafx.h"
#include "ILDisassembler.h"
#include "managed_function.h"

using namespace vanguard::instrumentation::managed;

// {CA487940-57D2-10BF-11B2-A3AD5A13CBC0}
const GUID CLSID_ExtensionHost =
{ 0xCA487940, 0x57D2, 0x10BF,{ 0x11, 0xB2, 0xA3, 0xAD, 0x5A, 0x13, 0xCB, 0xC0 } };

namespace ExtensionsHostCrossPlat
{

    // The existing ExtensionsHost had too much non-portable code to port to Linux.
    // In order to avoid destabilizing appinsights through a rewrite, this class
    // has been created to provide basic instrumentation engine host support for
    // production breakpoints.
    class ATL_NO_VTABLE CExtensionHost :
        public IInstrumentationMethod,
        public CModuleRefCount
    {
    public:
        CExtensionHost()
        {
#ifdef PLATFORM_UNIX
            PAL_Initialize(0, NULL);
#endif
        }

        CExtensionHost(const CExtensionHost&) = delete;

        DEFINE_DELEGATED_REFCOUNT_ADDREF(CExtensionHost)
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CExtensionHost)

        STDMETHOD(STDMETHODCALLTYPE) QueryInterface(REFIID riid, PVOID* ppvObject)
        {
            HRESULT hr = E_NOINTERFACE;

            hr = ImplQueryInterface(
                static_cast<IInstrumentationMethod*>(this),
                riid,
                ppvObject
            );

            return hr;
        }

        // IProfilerManagerHost methods
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize(IProfilerManager* pProfilerManager);

        virtual HRESULT STDMETHODCALLTYPE OnAppDomainCreated(IAppDomainInfo *pAppDomainInfo) { return S_OK; }

        virtual HRESULT STDMETHODCALLTYPE OnAppDomainShutdown(IAppDomainInfo *pAppDomainInfo) { return S_OK; }

        virtual HRESULT STDMETHODCALLTYPE OnAssemblyLoaded(IAssemblyInfo* pAssemblyInfo) { return S_OK; }

        virtual HRESULT STDMETHODCALLTYPE OnAssemblyUnloaded(IAssemblyInfo* pAssemblyInfo) { return S_OK; }

        virtual HRESULT STDMETHODCALLTYPE OnModuleLoaded(IModuleInfo* pModuleInfo);

        virtual HRESULT STDMETHODCALLTYPE OnModuleUnloaded(IModuleInfo* pModuleInfo) { return S_OK; }

        virtual HRESULT STDMETHODCALLTYPE OnShutdown();

        virtual HRESULT STDMETHODCALLTYPE ShouldInstrumentMethod(IMethodInfo* pMethodInfo, BOOL isRejit, BOOL* pbInstrument);

        virtual HRESULT STDMETHODCALLTYPE BeforeInstrumentMethod(IMethodInfo* pMethodInfo, BOOL isRejit) { return S_OK; }

        virtual HRESULT STDMETHODCALLTYPE InstrumentMethod(IMethodInfo* pMethodInfo, BOOL isRejit);

        virtual HRESULT STDMETHODCALLTYPE OnInstrumentationComplete(IMethodInfo* pMethodInfo, BOOL isRejit) { return S_OK; }

        virtual HRESULT STDMETHODCALLTYPE AllowInlineSite(IMethodInfo* pMethodInfoInlinee, IMethodInfo* pMethodInfoCaller, BOOL* pbAllowInline) { return S_OK; }

    private:

        HMODULE m_hmod = NULL;

    };

}