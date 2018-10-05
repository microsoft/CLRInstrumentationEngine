// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "CompositeType.h"

MicrosoftInstrumentationEngine::CCompositeType::CCompositeType(_In_ CorElementType type, _In_ IType* relatedType) : CType(type)
{
    DEFINE_REFCOUNT_NAME(CCompositeType);
    m_relatedType = CComPtr<IType>(relatedType);
}

HRESULT MicrosoftInstrumentationEngine::CCompositeType::GetRelatedType(_Out_ IType** type)
{
    IfNullRetPointer(type);
    return m_relatedType->QueryInterface<IType>(type);
}

HRESULT MicrosoftInstrumentationEngine::CCompositeType::AddToSignature(_In_ ISignatureBuilder* pSignatureBuilder)
{
    HRESULT hr = S_OK;
    IfFailRet(CType::AddToSignature(pSignatureBuilder));
    IfFailRet(m_relatedType->AddToSignature(pSignatureBuilder));
    return hr;
}