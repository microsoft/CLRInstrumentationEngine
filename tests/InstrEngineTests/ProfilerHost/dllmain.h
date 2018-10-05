// Copyright (c) Microsoft Corporation. All rights reserved.
// 

// dllmain.h : Declaration of module class.

const IID LIBID_ProfilerManagerHostLib = { 0x5FF6580E, 0xC81E, 0x45D2, { 0xA4, 0x8B, 0xE0, 0x27, 0x8E, 0x3C, 0x26, 0x80 } };

class CProfilerHostModule : public ATL::CAtlDllModuleT< CProfilerHostModule >
{
public :
    DECLARE_LIBID(LIBID_ProfilerManagerHostLib)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PROFILERHOST, "{5F0AEDDF-3D38-4DB4-9AE1-629BA7338C2E}")
};

extern class CProfilerHostModule _AtlModule;
