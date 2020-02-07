// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "InstrumentationMethodSetting.h"

namespace MicrosoftInstrumentationEngine
{
    class CConfigurationSource :
        public CModuleRefCount,
        public IUnknown
    {
    private:
        CComBSTR m_bstrPath;
        std::vector<ATL::CComPtr<IInstrumentationMethodSetting>> m_settings;

    public:
        CConfigurationSource(_In_ LPCWSTR wszPath) noexcept;

    // IUnknown
    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CConfigurationSource);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CConfigurationSource);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void** ppvObject) override;

    public:
        STDMETHOD(AddSetting)(_In_ LPCWSTR wszName, _In_ LPCWSTR wszValue);
        STDMETHOD(EnumSettings)(_Outptr_ IEnumInstrumentationMethodSettings** ppEnum);
        STDMETHOD(GetPath)(_Out_ BSTR* pbstrPath) const;
    };
}