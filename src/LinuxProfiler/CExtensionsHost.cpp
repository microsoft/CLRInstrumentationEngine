// Copyright (c) Microsoft Corporation. All rights reserved.
// 

// CExtensionsHost.cpp : Implementation of CExtensionHost

#include "stdafx.h"
#include <unistd.h>
#include "CExtensionsHost.h"


HRESULT ExtensionsHostCrossPlat::CExtensionHost::Initialize(
    _In_ IProfilerManager* pProfilerManager
)
{
    CComPtr<ICorProfilerInfo> pCorProfilerInfo;
    pProfilerManager->GetCorProfilerInfo((IUnknown**)&pCorProfilerInfo);
    pCorProfilerInfo->SetEventMask(COR_PRF_MONITOR_EXCEPTIONS | COR_PRF_ENABLE_STACK_SNAPSHOT);
    return S_OK;
}

HRESULT ExtensionsHostCrossPlat::CExtensionHost::OnModuleLoaded(IModuleInfo* pModuleInfo)
{
    HRESULT hr = S_OK;

    CComBSTR bstrModuleName;
    IfFailRet(pModuleInfo->GetModuleName(&bstrModuleName));

    CComBSTR bstrModulePath;
    IfFailRet(pModuleInfo->GetFullPath(&bstrModulePath));

    tstringstream dllPath;
    dllPath << (LPWSTR)bstrModulePath;

    tstring pdbPath = dllPath.str();
    pdbPath = pdbPath.substr(0, pdbPath.find_last_of(_T('.'))) + _T(".pdb");

    string filePathChar(pdbPath.begin(), pdbPath.end());
    FILE * pFile;
    pFile = fopen(filePathChar.c_str(), "r");

    if (pFile == NULL)
    {
        return S_OK;
    }
    else
    {
        fclose(pFile);
    }

    tstringstream pathBuilder;
    pathBuilder <<_T("/home/maban/projects/XPlatPdbReader/bin/x64/Debug/libXPlatPdbReader.so");

    if (m_hmod == NULL)
    {
        m_hmod = ::LoadLibrary(pathBuilder.str().c_str());
    }

    if (m_hmod == NULL)
    {
        auto error = ::GetLastError();
        // Failed to load the module.
        return HRESULT_FROM_WIN32(error);
    }

    typedef int(__stdcall* ReadPDB)(const char* path);

    ReadPDB pfnReadPdb = (ReadPDB)GetProcAddress(m_hmod, "ReadPDB");
    if (!pfnReadPdb)
    {
        auto error = ::GetLastError();
        FreeLibrary(m_hmod);
        return HRESULT_FROM_WIN32(error);
    }

    int methodCount = pfnReadPdb(filePathChar.c_str());

    return hr;
}
