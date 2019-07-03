// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ModifierType.h"

namespace MicrosoftInstrumentationEngine
{
    CModifierType::CModifierType(_In_ CorElementType type, _In_ mdToken token) : CType(type), m_token(token)
    {
        DEFINE_REFCOUNT_NAME(CModifierType);
    }

    HRESULT CModifierType::AddToSignature(_In_ ISignatureBuilder* pSignatureBuilder)
    {
        HRESULT hr = S_OK;

        IfFailRet(CType::AddToSignature(pSignatureBuilder));
        IfFailRet(pSignatureBuilder->AddToken(m_token));

        return hr;
    }
}