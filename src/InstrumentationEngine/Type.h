// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "DataContainer.h"
#include "InstrumentationEngine.h"

namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("6FC96859-ED89-4D9F-A7C9-1DAD7EC35F67"))
    CType : public IType, public CDataContainer
    {
        friend class CTypeCreator;

        private:
            CorElementType m_type;
            bool m_isSentinel;
            bool m_isPinned;
            std::vector<CComPtr<IType>> m_modifiers;
        public:
            DEFINE_DELEGATED_REFCOUNT_ADDREF(CType);
            DEFINE_DELEGATED_REFCOUNT_RELEASE(CType);
            STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
            {
                return ImplQueryInterface(
                    static_cast<IType*>(this),
                    static_cast<IDataContainer*>(static_cast<CDataContainer*>(this)),
                    riid,
                    ppvObject
                    );
            }
        // IType
        public:
            STDMETHOD(GetCorElementType)(_Out_ CorElementType* pCorType) override;
            STDMETHOD(AddToSignature)(_In_ ISignatureBuilder* pSignatureBuilder) override;
            STDMETHOD(IsPrimitive)(_Out_ BOOL* pIsPrimitive) override;
            STDMETHOD(IsArray)(_Out_ BOOL* pIsArray) override;
            STDMETHOD(IsClass)(_Out_ BOOL* pIsClass) override;
            STDMETHOD(GetName)(_Out_ BSTR* pbstrName) override;
        public:
            //TODO Scoping these at Type is probably too broad. We should consider moving modifiers/pinned/sentienl to IMethodParameter instead
            //of the type itself. There is also a difference between things like [Mod]PTR[Type] and PTR[Mod][Type]. Both can exist, apparantly due to
            //a c++ compiler bug.
            HRESULT SetIsPinned(_In_ bool isPinned);
            HRESULT SetIsSentinel(_In_ bool isSentinel);
            HRESULT SetModifers(const _In_ std::vector<CComPtr<IType>>& modifiers);
        protected:
            CType(_In_ CorElementType type);
    };
}