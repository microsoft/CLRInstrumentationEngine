// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "InstrumentationMethodSetting.h"

namespace MicrosoftInstrumentationEngine
{
    CInstrumentationMethodSetting::CInstrumentationMethodSetting(_In_ LPCWSTR wszName, _In_ LPCWSTR wszValue) noexcept :
        m_bstrName(wszName),
        m_bstrValue(wszValue)
    {
    }

    STDMETHODIMP CInstrumentationMethodSetting::QueryInterface(_In_ REFIID riid, _Out_ void** ppvObject)
    {
        return ImplQueryInterface(
            static_cast<IInstrumentationMethodSetting*>(this),
            riid,
            ppvObject
            );
    }

    STDMETHODIMP CInstrumentationMethodSetting::GetName(_Out_ BSTR* pbstrName)
    {
        return m_bstrName.CopyTo(pbstrName);
    }

    STDMETHODIMP CInstrumentationMethodSetting::GetValue(_Out_ BSTR* pbstrValue)
    {
        return m_bstrValue.CopyTo(pbstrValue);
    }
}