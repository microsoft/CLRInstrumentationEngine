// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "InstrumentationEngine.h"
#include "Type.h"
#include <vector>

namespace MicrosoftInstrumentationEngine
{
    class CFunctionType : public CType
    {
        friend class CTypeCreator;
    private:
        CorCallingConvention m_callingConvention;
        CComPtr<IType> m_pReturnType;
        std::vector<CComPtr<IType>> m_parameters;
        DWORD m_dwGenericParameterCount;
    // IType
    public:
        STDMETHOD(AddToSignature)(_In_ ISignatureBuilder* pSignatureBuilder);
    protected:
        CFunctionType(_In_ CorCallingConvention callingConvention, _In_ IType* pReturnType, _In_ const std::vector<CComPtr<IType>>& parameters, _In_ DWORD dwGenericParameterCount);
    };
}