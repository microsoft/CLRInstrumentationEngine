// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "InstrumentationEngine.h"
#include "CompositeType.h"

namespace MicrosoftInstrumentationEngine
{
    class CGenericInstance : public CCompositeType
    {
        friend class CTypeCreator;

        private:
            std::vector<CComPtr<IType>> m_genericParameters;
        protected:
            CGenericInstance(_In_ IType* typeDefinition, _In_ const std::vector<IType*>& genericParameters);
        // IType
        public:
            STDMETHOD(AddToSignature)(_In_ ISignatureBuilder* pSignatureBuilder);
    };
}
