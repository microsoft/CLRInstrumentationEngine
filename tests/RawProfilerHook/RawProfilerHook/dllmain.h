// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// dllmain.h : Declaration of module class.

class CRawProfilerHookModule : public ATL::CAtlDllModuleT< CRawProfilerHookModule >
{
public :
	DECLARE_LIBID(LIBID_RawProfilerHookLib)
};

extern class CRawProfilerHookModule _AtlModule;
