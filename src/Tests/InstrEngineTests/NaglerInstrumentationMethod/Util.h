// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "stdafx.h"

namespace MicrosoftInstrumentationEngine
{
    template<class T> class CMetadataEnumCloser final
    {
    private:
        CComPtr<T> m_pMetadataImport;
        HCORENUM m_hEnum;

    public:
        CMetadataEnumCloser(T* pImport, HCORENUM hEnum)
            : m_pMetadataImport(pImport),
            m_hEnum(hEnum)
        {
            if (m_pMetadataImport == nullptr)
            {
                AssertFailed("m_pMetadataImport is null. Potential memory leak");
            }
        }

        ~CMetadataEnumCloser()
        {
            CloseEnum();
        }

        HCORENUM* Get()
        {
            return &m_hEnum;
        }

        void Reset(HCORENUM hEnum)
        {
            CloseEnum();
            m_hEnum = hEnum;
        }

    private:
        void CloseEnum()
        {
            if (m_hEnum != nullptr)
            {
                m_pMetadataImport->CloseEnum(m_hEnum);
                m_hEnum = nullptr;
            }
        }
    };
}