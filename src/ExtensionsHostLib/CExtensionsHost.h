// Copyright (c) Microsoft Corporation. All rights reserved.
//

// CExtensionsHost.h : Declaration of the CExtensionsHost

#pragma once

#include "stdafx.h"

#ifndef PLATFORM_UNIX

#include "../ExtensionsCommon/ProfilerManagerHostBase.h"
#include "../ExtensionsCommon/ModuleHandle.h"

#endif

class CExtensionsHost :
#ifndef PLATFORM_UNIX
    public CProfilerManagerHostBase,
#else
    public IProfilerManagerHost,
#endif
    public MicrosoftInstrumentationEngine::CModuleRefCount
{

#ifndef PLATFORM_UNIX
private:
    Agent::Io::CModuleHandle m_hRawHookModule;

public:
    CExtensionsHost() noexcept
    {
    }

#else
public:
    CExtensionsHost()
    {
        PAL_Initialize(0, NULL);
    }
#endif

    CExtensionsHost(const CExtensionsHost&) = delete;

    DEFINE_DELEGATED_REFCOUNT_ADDREF(CExtensionsHost);
    DEFINE_DELEGATED_REFCOUNT_RELEASE(CExtensionsHost);
    STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void** ppvObject) override
    {
        HRESULT hr = E_NOINTERFACE;

        hr = MicrosoftInstrumentationEngine::ImplQueryInterface(
            static_cast<IProfilerManagerHost*>(this),
            riid,
            ppvObject
        );

        return hr;
    }

#ifndef PLATFORM_UNIX
    _Check_return_ HRESULT InternalInitialize(
        _In_ const IProfilerManagerSptr& sptrHost) override final;

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }

    virtual ~CExtensionsHost() {};
#else
    // IProfilerManagerHost methods
    public:
        STDMETHOD(Initialize)(_In_ IProfilerManager* pProfilerManager) override;

    private:
        static void SetLoggingFlags(_In_ IProfilerManagerLogging* pLogger);
#endif

};