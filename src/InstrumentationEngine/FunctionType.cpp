// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "FunctionType.h"

namespace MicrosoftInstrumentationEngine
{
    CFunctionType::CFunctionType(_In_ CorCallingConvention callingConvention, _In_ IType* pReturnType, _In_ const std::vector<CComPtr<IType>>& parameters, _In_ DWORD dwGenericParameterCount)
        : CType(ELEMENT_TYPE_FNPTR), m_callingConvention(callingConvention), m_pReturnType(pReturnType), m_parameters(parameters), m_dwGenericParameterCount(dwGenericParameterCount)
    {
        DEFINE_REFCOUNT_NAME(CFunctionType);
    }

    HRESULT CFunctionType::AddToSignature(_In_ ISignatureBuilder* pSignatureBuilder)
    {
        HRESULT hr = S_OK;
        IfFailRet(CType::AddToSignature(pSignatureBuilder));

        IfFailRet(pSignatureBuilder->AddData((const BYTE*)&m_callingConvention, 1));
        if (m_callingConvention & IMAGE_CEE_CS_CALLCONV_GENERIC)
        {
            IfFailRet(pSignatureBuilder->Add(m_dwGenericParameterCount));
        }
        IfFailRet(pSignatureBuilder->Add((DWORD)(m_parameters.size())));
        IfFailRet(m_pReturnType->AddToSignature(pSignatureBuilder));
        for (IType* parameter : m_parameters)
        {
            IfFailRet(parameter->AddToSignature(pSignatureBuilder));
        }
        return hr;
    }

}