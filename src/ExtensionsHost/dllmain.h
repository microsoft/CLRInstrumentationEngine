// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// dllmain.h : Declaration of module class.

class CExtensionsHostModule : public ATL::CAtlDllModuleT< CExtensionsHostModule >
{
public :
	DECLARE_LIBID(LIBID_ExtensionsHostLib)
};

extern class CExtensionsHostModule _AtlModule;
