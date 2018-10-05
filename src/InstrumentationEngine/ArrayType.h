// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "InstrumentationEngine.h"
#include "CompositeType.h"

namespace MicrosoftInstrumentationEngine
{
    class CArrayType : public CCompositeType
    {
        friend class CTypeCreator;

        private:
            ULONG m_rank;
            std::vector<ULONG> m_counts;
            std::vector<ULONG> m_bounds;
        protected:
            CArrayType(_In_ IType* relatedType, _In_ ULONG rank, _In_ const std::vector<ULONG>& counts, _In_ const std::vector<ULONG>& bounds);
        // IType
        public:
            STDMETHOD(AddToSignature)(_In_ ISignatureBuilder* pSignatureBuilder);

    };
}