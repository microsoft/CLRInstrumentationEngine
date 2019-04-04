// Copyright (c) Microsoft Corporation. All rights reserved.
//

// CExtensionsHost.h : Declaration of the CExtensionsHost

#pragma once

#include "../stdafx.h"

class CExtensionsHost :
    public IProfilerManagerHost,
    public MicrosoftInstrumentationEngine::CModuleRefCount
{

public:
    CExtensionsHost()
    {
        PAL_Initialize(0, NULL);
    }

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

    // IProfilerManagerHost methods
    public:
        STDMETHOD(Initialize)(_In_ IProfilerManager* pProfilerManager) override;
};