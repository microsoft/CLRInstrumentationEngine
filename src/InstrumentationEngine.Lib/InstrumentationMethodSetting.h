// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace MicrosoftInstrumentationEngine
{
    class CInstrumentationMethodSetting :
        public CModuleRefCount,
        public IInstrumentationMethodSetting
    {
    private:
        CComBSTR m_bstrName;
        CComBSTR m_bstrValue;

    public:
        CInstrumentationMethodSetting(_In_ LPCWSTR wszName, _In_ LPCWSTR wszValue) noexcept;

        // IUnknown
    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CInstrumentationMethodSetting);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CInstrumentationMethodSetting);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void** ppvObject) override;

        // IInstrumentationMethodSetting
    public:
        STDMETHOD(GetName)(_Out_ BSTR* pbstrName) override;
        STDMETHOD(GetValue)(_Out_ BSTR* pbstrValue) override;
    };
}