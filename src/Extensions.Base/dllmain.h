// Copyright (c) Microsoft Corporation. All rights reserved.
//

// dllmain.h : Declaration of module class.

class CHostExtensionModule : public ATL::CAtlDllModuleT< CHostExtensionModule >
{
public :
	DECLARE_LIBID(LIBID_HostExtensionLib)
};

extern class CHostExtensionModule _AtlModule;
