// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Type.h"
#include "InstrumentationEngine.h"

namespace MicrosoftInstrumentationEngine
{
    class CCompositeType : public ICompositeType, public CType
    {
        friend class CTypeCreator;

        private:
            CComPtr<IType> m_relatedType;
        public:
            DEFINE_DELEGATED_REFCOUNT_ADDREF(CCompositeType);
            DEFINE_DELEGATED_REFCOUNT_RELEASE(CCompositeType);
            STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
            {
                return ImplQueryInterface(
                    static_cast<IType*>(this),
                    static_cast<ICompositeType*>(this),
                    static_cast<IDataContainer*>(static_cast<CDataContainer*>(this)),
                    riid,
                    ppvObject
                );
            }
        // ICompositeType
        public:
            STDMETHOD(GetRelatedType)(_Out_ IType** type) override;
        // IType
        public:
            STDMETHOD(AddToSignature)(_In_ ISignatureBuilder* pSignatureBuilder) override;
        protected:
            CCompositeType(_In_ CorElementType type, _In_ IType* relatedType);
    };
}