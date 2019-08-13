// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "InstrumentationEngine.h"
#include "Type.h"

namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("1D5C1393-DC7E-4FEF-8A9D-A3DAF7A55C6E"))
    CGenericParameterType : public CType, public IGenericParameterType
    {
        friend class CTypeCreator;
        private:
            ULONG m_position;
        public:
            DEFINE_DELEGATED_REFCOUNT_ADDREF(CGenericParameterType);
            DEFINE_DELEGATED_REFCOUNT_RELEASE(CGenericParameterType);
            STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
            {
                return ImplQueryInterface(
                    static_cast<IGenericParameterType*>(this),
                    static_cast<IType*>(this),
                    riid,
                    ppvObject
                    );
            }
        // IGenericParameterType
        public:
            STDMETHOD(GetPosition)(_Out_ ULONG* pPosition) override;
        // IType
        public:
            STDMETHOD(AddToSignature)(_In_ ISignatureBuilder* pSignatureBuilder) override;
        protected:
            CGenericParameterType(_In_ CorElementType type, _In_ ULONG position);
    };
}