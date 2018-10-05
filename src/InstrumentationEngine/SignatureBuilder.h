// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "InstrumentationEngine.h"

namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("F574823E-4863-4013-A4EA-C6D9943246E6"))
    CSignatureBuilder : public ISignatureBuilder, public CModuleRefCount
    {
        private:
            BYTE* m_memory;
            DWORD m_used;
            DWORD m_capacity;

        public:
            DEFINE_DELEGATED_REFCOUNT_ADDREF(CSignatureBuilder);
            DEFINE_DELEGATED_REFCOUNT_RELEASE(CSignatureBuilder);
            STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
            {
                return ImplQueryInterface(
                    static_cast<ISignatureBuilder*>(this),
                    riid,
                    ppvObject
                    );
            }
        public:
            CSignatureBuilder();
            ~CSignatureBuilder();
        // ISignatureBuilder
        public:
            STDMETHOD(Add)(_In_ ULONG x) override;
            STDMETHOD(AddSignedInt)(_In_ LONG y) override;
            STDMETHOD(AddToken)(_In_ mdToken t) override;
            STDMETHOD(AddElementType)(_In_ CorElementType x) override;
            STDMETHOD(AddData)(_In_ const BYTE* memory, _In_ DWORD cbSize) override;
            STDMETHOD(AddSignature)(_In_ ISignatureBuilder* pSignature) override;
            STDMETHOD(Clear)() override;
            STDMETHOD(GetCorSignature)(_In_ DWORD cbBuffer, _Inout_opt_ BYTE* pCorSignature, _Out_opt_ DWORD* pcbSignature) override;
            STDMETHOD(GetCorSignaturePtr)(_Out_ const BYTE** ppCorSignature) override;
            STDMETHOD(GetSize)(_Out_ DWORD* cbSize) override;
        private:
            HRESULT EnsureCapacity(_In_ DWORD needed);
    };
}

