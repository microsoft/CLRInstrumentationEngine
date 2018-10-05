// Copyright (c) Microsoft Corporation. All rights reserved.
// 

// dllmain.h : Declaration of module class.

class CExtensionsHostModule : public ATL::CAtlDllModuleT< CExtensionsHostModule >
{
public :
	DECLARE_LIBID(LIBID_ExtensionsHostLib)
};

extern class CExtensionsHostModule _AtlModule;
