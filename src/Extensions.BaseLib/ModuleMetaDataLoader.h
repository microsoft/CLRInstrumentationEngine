// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once
#include "ExtensionsCommon\ProfilerHostServices.h"

class CModuleMetaDataLoader
    : public CProfilerHostServices
{
private:
    const std::wstring m_strModulePath;

    std::shared_ptr<HINSTANCE__> m_spSourceImage;
    ATL::CComPtr<IMetaDataImport2> m_spMetaDataImport;


public:
    CModuleMetaDataLoader(_In_ const std::wstring& modulePath);
    virtual HRESULT Load();

	_Ret_notnull_ LPCBYTE* GetImageBaseAddress() const;
	ATL::CComPtr<IMetaDataImport2>& GetMetaDataImport();
};
