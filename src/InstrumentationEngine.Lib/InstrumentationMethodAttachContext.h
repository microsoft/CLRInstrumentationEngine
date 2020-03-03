// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ConfigurationSource.h"

namespace MicrosoftInstrumentationEngine
{
    class CInstrumentationMethodAttachContext :
        public CModuleRefCount,
        public IInstrumentationMethodAttachContext
    {
    private:
        ATL::CComPtr<IEnumInstrumentationMethodSettings> m_pSettingsEnum;

    public:
        CInstrumentationMethodAttachContext(_In_ IEnumInstrumentationMethodSettings* pSettingsEnum) noexcept;

        // IUnknown
    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CInstrumentationMethodSetting);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CInstrumentationMethodSetting);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void** ppvObject) override;

        // IInstrumentationMethodAttachContext
    public:
        STDMETHOD(EnumSettings)(_Outptr_result_maybenull_ IEnumInstrumentationMethodSettings** ppEnum) override;
    };
}