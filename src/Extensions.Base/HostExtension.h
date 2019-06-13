// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "Extensions.BaseLib/HostExtensionBase.h"
#include "HostExtension_i.h"

// CHostExtension

class ATL_NO_VTABLE CHostExtension :
	public ATL::CComObjectRootEx<ATL::CComMultiThreadModelNoCS>,
	public ATL::CComCoClass<CHostExtension, &CLSID_HostExtension>,
	public CHostExtensionBase
{
	CHostExtension(const CHostExtension&) = delete;
public:
	CHostExtension() {}

	virtual ~CHostExtension() {};

	DECLARE_NO_REGISTRY()

	DECLARE_NOT_AGGREGATABLE(CHostExtension)

	BEGIN_COM_MAP(CHostExtension)
		COM_INTERFACE_ENTRY(IInstrumentationMethod)
		//COM_INTERFACE_ENTRY(IProfilerManagerLoggingHost)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct() { return S_OK; }
	void FinalRelease() {}
};

OBJECT_ENTRY_AUTO(CLSID_HostExtension, CHostExtension)
