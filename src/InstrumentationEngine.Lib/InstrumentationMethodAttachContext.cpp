// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "InstrumentationMethodAttachContext.h"

namespace MicrosoftInstrumentationEngine
{
    CInstrumentationMethodAttachContext::CInstrumentationMethodAttachContext(_In_ IEnumInstrumentationMethodSettings* pSettingsEnum) noexcept :
        m_pSettingsEnum(pSettingsEnum)
    {
    }

    STDMETHODIMP CInstrumentationMethodAttachContext::QueryInterface(_In_ REFIID riid, _Out_ void** ppvObject)
    {
        return ImplQueryInterface(
            static_cast<IInstrumentationMethodAttachContext*>(this),
            riid,
            ppvObject
            );
    }

    STDMETHODIMP CInstrumentationMethodAttachContext::EnumSettings(_Outptr_result_maybenull_ IEnumInstrumentationMethodSettings** ppEnum)
    {
        IfNullRetPointer(ppEnum);

        return m_pSettingsEnum.CopyTo(ppEnum);
    }
}