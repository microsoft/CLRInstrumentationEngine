// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "Type.h"
#include "InstrumentationEngine.h"

namespace MicrosoftInstrumentationEngine
{
    class CTokenType : public ITokenType, public CType
    {
        friend class CTypeCreator;

        private:
            mdToken m_token;
            CComPtr<IModuleInfo> m_pOwningModule;
            CComBSTR m_name;

        public:
            DEFINE_DELEGATED_REFCOUNT_ADDREF(CTokenType);
            DEFINE_DELEGATED_REFCOUNT_RELEASE(CTokenType);
            STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
            {
                return ImplQueryInterface(
                    static_cast<IType*>(this),
                    static_cast<ITokenType*>(this),
                    static_cast<IDataContainer*>(static_cast<CDataContainer*>(this)),
                    riid,
                    ppvObject
                    );
            }

        // ITokenType methods.
        public:
            STDMETHOD(GetToken)(_Out_ mdToken* token) override;
            STDMETHOD(GetOwningModule)(_Out_ IModuleInfo** ppOwningModule) override;

        public:
            STDMETHOD(GetName)(_Out_ BSTR* pbstrName) override;
            STDMETHOD(AddToSignature)(_In_ ISignatureBuilder* pSignatureBuilder) override;
        protected:
            CTokenType(_In_ IModuleInfo* module, _In_ mdToken token, _In_ CorElementType type);
    };
}
