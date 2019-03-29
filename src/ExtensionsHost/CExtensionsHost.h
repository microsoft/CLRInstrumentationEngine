// Copyright (c) Microsoft Corporation. All rights reserved.
//

// CExtensionsHost.h : Declaration of the CExtensionsHost

#pragma once

#include "stdafx.h"

#include "../ExtensionsCommon/ProfilerManagerHostBase.h"
#include "../ExtensionsCommon/ModuleHandle.h"

class CExtensionsHost :
    public CProfilerManagerHostBase,
    public MicrosoftInstrumentationEngine::CModuleRefCount
{
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
        return MicrosoftInstrumentationEngine::ImplQueryInterface(
            static_cast<IProfilerManagerHost*>(this),
            riid,
            ppvObject
        );
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