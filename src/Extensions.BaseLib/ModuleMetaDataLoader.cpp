// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "ModuleMetaDataLoader.h"
#include <metahost.h>

CModuleMetaDataLoader::CModuleMetaDataLoader(_In_ const std::wstring& modulePath)
    : m_strModulePath(modulePath)
{
}

HRESULT CModuleMetaDataLoader::Load()
{
    HRESULT hr = S_OK;

    ATL::CComPtr<IMetaDataDispenserEx> pDisp;

    ATL::CComPtr<ICLRMetaHost> pMetaHost;
    ATL::CComPtr<ICLRRuntimeInfo> pRuntime;
    IfFailRet(CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, reinterpret_cast<void **>(&pMetaHost)));
    IfFailRet(pMetaHost->GetRuntime(L"v4.0.30319", IID_ICLRRuntimeInfo, reinterpret_cast<void **>(&pRuntime)));
    IfFailRet(pRuntime->GetInterface(CLSID_CorMetaDataDispenser, IID_IMetaDataDispenserEx, reinterpret_cast<void **>(&pDisp)));

    IfFailRet(pDisp->OpenScope(m_strModulePath.c_str(), ofReadOnly, IID_IMetaDataImport2, reinterpret_cast<IUnknown **>(&m_spMetaDataImport)));

    // Loads the image with section alignment, but doesn't do any of the other steps to ready an image to run
    // This is sufficient to read IL code from the image without doing RVA -> file offset mapping
    m_spSourceImage = std::shared_ptr<HINSTANCE__>(LoadLibraryEx(m_strModulePath.c_str(), NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE), FreeLibrary);
    if (m_spSourceImage == nullptr)
    {
        return HRESULT_FROM_WIN32(::GetLastError());
    }

    return hr;
}

_Ret_notnull_ LPCBYTE* CModuleMetaDataLoader::GetImageBaseAddress() const
{
    //Not sure if there is a better way to get the base address of a module loaded via LoadLibrary?
    //We are cheating a bit knowing that module handles are actually base addresses with a few bits OR'ed in
    return reinterpret_cast<LPCBYTE*>((ULONGLONG)m_spSourceImage.get() & (~2));
}

ATL::CComPtr<IMetaDataImport2>& CModuleMetaDataLoader::GetMetaDataImport()
{
    return m_spMetaDataImport;
}