// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "GenericParameterType.h"

MicrosoftInstrumentationEngine::CGenericParameterType::CGenericParameterType(_In_ CorElementType type, _In_ ULONG position) : CType(type), m_position(position)
{
    DEFINE_REFCOUNT_NAME(CGenericParameterType);
}

HRESULT MicrosoftInstrumentationEngine::CGenericParameterType::GetPosition(_Out_ ULONG* pPosition)
{
    IfNullRetPointer(pPosition);
    *pPosition = m_position;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CGenericParameterType::AddToSignature(_In_ ISignatureBuilder* pSignatureBuilder)
{
    HRESULT hr = S_OK;
    IfFailRet(CType::AddToSignature(pSignatureBuilder));
    IfFailRet(pSignatureBuilder->Add(m_position));
    return hr;
}