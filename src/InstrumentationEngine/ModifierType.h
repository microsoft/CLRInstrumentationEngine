// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "InstrumentationEngine.h"
#include "Type.h"
#include <cor.h>

namespace MicrosoftInstrumentationEngine
{
    class CModifierType : public CType
    {
        friend class CTypeCreator;

        private:
            mdToken m_token;
        // IType
        public:
            STDMETHOD(AddToSignature)(_In_ ISignatureBuilder* pSignatureBuilder);
        protected:
            CModifierType(_In_ CorElementType type, _In_ mdToken token);
    };
}