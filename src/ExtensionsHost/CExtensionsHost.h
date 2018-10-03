// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// CExtensionsHost.h : Declaration of the CExtensionHost

#pragma once

#include "ExtensionsHost_i.h"

#include "ExtensionsCommon/ProfilerManagerHostBase.h"

class ATL_NO_VTABLE CExtensionHost :
    public ATL::CComObjectRootEx<ATL::CComMultiThreadModelNoCS>,
    public ATL::CComCoClass<CExtensionHost, &CLSID_ExtensionHost>,
    public ISupportErrorInfo,
    public CProfilerManagerHostBase
{
    Agent::Io::CModuleHandle m_hRawHookModule;
public:
    CExtensionHost() noexcept
    {
    }

    CExtensionHost(const CExtensionHost&) = delete;

DECLARE_NO_REGISTRY()

DECLARE_NOT_AGGREGATABLE(CExtensionHost)

BEGIN_COM_MAP(CExtensionHost)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
    COM_INTERFACE_ENTRY(IProfilerManagerHost)
END_COM_MAP()

// ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    _Check_return_ HRESULT InternalInitialize(
        _In_ const IProfilerManagerSptr& sptrHost) override final;

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }

    virtual ~CExtensionHost() {};
};

OBJECT_ENTRY_AUTO(__uuidof(ExtensionHost), CExtensionHost)