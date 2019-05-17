// Copyright (c) Microsoft Corporation. All rights reserved.
//

// CExtensionsHost.h : Declaration of the CExtensionsHost

#pragma once

#include "../stdafx.h"

#include "../../ExtensionsCommon/ProfilerManagerHostBase.h"
#include "../../ExtensionsCommon/ModuleHandle.h"
#include "../../InstrumentationEngine.Lib/ImplQueryInterface.h"
#include "../../InstrumentationEngine.Lib/refcount.h"

class CExtensionsHost :
    public CProfilerManagerHostBase,
    public MicrosoftInstrumentationEngine::CModuleRefCount
{
private:
    Agent::Io::CModuleHandle m_hRawHookModule;

public:
    CExtensionsHost() noexcept
    {
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
};