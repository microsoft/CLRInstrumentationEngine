// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ConfigurationSource.h"
#include "EnumeratorImpl.h"

namespace MicrosoftInstrumentationEngine
{
    CConfigurationSource::CConfigurationSource(_In_ LPCWSTR wszPath) noexcept :
        m_bstrPath(wszPath)
    {
    }

    STDMETHODIMP CConfigurationSource::QueryInterface(_In_ REFIID riid, _Out_ void** ppvObject)
    {
        return ImplQueryInterface(
            static_cast<IUnknown*>(this),
            riid,
            ppvObject
            );
    }

    STDMETHODIMP CConfigurationSource::AddSetting(_In_ LPCWSTR wszName, _In_ LPCWSTR wszValue)
    {
        CComPtr<CInstrumentationMethodSetting> pSetting;
        pSetting.Attach(new (nothrow) CInstrumentationMethodSetting(wszName, wszValue));
        IfFalseRet(nullptr != pSetting, E_OUTOFMEMORY);

        m_settings.push_back(pSetting.p);

        return S_OK;
    }

    STDMETHODIMP CConfigurationSource::EnumSettings(_Outptr_ IEnumInstrumentationMethodSettings** ppEnum)
    {
        IfNullRetPointer(ppEnum);

        *ppEnum = nullptr;

        HRESULT hr = S_OK;

        CComPtr<CEnumerator<IEnumInstrumentationMethodSettings, IInstrumentationMethodSetting>> pEnumerator;
        pEnumerator.Attach(new (nothrow) CEnumerator<IEnumInstrumentationMethodSettings, IInstrumentationMethodSetting>());
        IfFalseRet(nullptr != pEnumerator, E_OUTOFMEMORY);

        IfFailRet(pEnumerator->Initialize(m_settings));

        *ppEnum = pEnumerator.Detach();

        return S_OK;
    }

    STDMETHODIMP CConfigurationSource::GetPath(_Out_ BSTR* pbstrPath) const
    {
        return m_bstrPath.CopyTo(pbstrPath);
    }
}