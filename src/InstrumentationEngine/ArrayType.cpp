// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ArrayType.h"

MicrosoftInstrumentationEngine::CArrayType::CArrayType(_In_ IType* relatedType, _In_ ULONG rank, _In_ const std::vector<ULONG>& counts, _In_ const std::vector<ULONG>& bounds)
    : CCompositeType(ELEMENT_TYPE_ARRAY, relatedType), m_rank(rank), m_counts(counts), m_bounds(bounds)
{
    DEFINE_REFCOUNT_NAME(CArrayType);
}

HRESULT MicrosoftInstrumentationEngine::CArrayType::AddToSignature(_In_ ISignatureBuilder* pSignatureBuilder)
{
    HRESULT hr = S_OK;
    IfFailRet(CCompositeType::AddToSignature(pSignatureBuilder));

    IfFailRet(pSignatureBuilder->Add(m_rank));
    IfFailRet(pSignatureBuilder->Add((DWORD)(m_counts.size())));
    for (ULONG count : m_counts)
    {
        IfFailRet(pSignatureBuilder->Add(count));
    }
    IfFailRet(pSignatureBuilder->Add((DWORD)(m_bounds.size())));
    for (ULONG bound: m_bounds)
    {
        IfFailRet(pSignatureBuilder->Add(bound));
    }
    return hr;
}
