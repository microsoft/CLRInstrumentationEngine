// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "GenericInstanceType.h"

MicrosoftInstrumentationEngine::CGenericInstance::CGenericInstance(_In_ IType* typeDefinition, _In_ const std::vector<IType*>& genericParameters) : CCompositeType(ELEMENT_TYPE_GENERICINST, typeDefinition)
{
    DEFINE_REFCOUNT_NAME(CGenericInstance);
    m_genericParameters.reserve(genericParameters.size());
    for (IType* type : genericParameters)
    {
        m_genericParameters.push_back(CComPtr<IType>(type));
    }
}

HRESULT MicrosoftInstrumentationEngine::CGenericInstance::AddToSignature(_In_ ISignatureBuilder* pSignatureBuilder)
{
    HRESULT hr = S_OK;
    IfFailRet(CCompositeType::AddToSignature(pSignatureBuilder));
    IfFailRet(pSignatureBuilder->Add((DWORD)(m_genericParameters.size())));
    for (IType* genericParameter : m_genericParameters)
    {
        IfFailRet(genericParameter->AddToSignature(pSignatureBuilder));
    }
    return hr;
}