// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "InstrumentationEngine.h"
#include "DataContainer.h"
#include <vector>

namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("353F806F-6563-40E0-8EBE-B93A58C0145F"))
    CLocalVariableCollection : public ILocalVariableCollection2, public CDataContainer
    {
        private:
            //It is important that this pointer be non-owning. The lifetime of this class is tied to the lifetime of the
            //MethodInfo class, and any AddRefs here just extend the life of both objects.
            IMethodInfo* m_pOwningMethod;
            std::vector<CComPtr<IType>> m_typesToAdd;
            DWORD m_originalLocalsCount;
            bool m_initialized;
            std::vector<BYTE> m_replacementSig;
            bool m_bReadOnly;

        public:
            ULONG __stdcall AddRef() override
            {
                return m_pOwningMethod->AddRef();
            }
            ULONG __stdcall Release() override
            {
                return m_pOwningMethod->Release();
            }
            STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
            {
                return ImplQueryInterface(
                    static_cast<ILocalVariableCollection*>(this),
                    static_cast<ILocalVariableCollection2*>(this),
                    static_cast<IDataContainer*>(static_cast<CDataContainer*>(this)),
                    riid,
                    ppvObject
                    );
            }
        public:
            CLocalVariableCollection(_In_ IMethodInfo* pOwningMethod, _In_ bool bReadOnly);

        // ILocalVariableCollection
        public:
            virtual HRESULT __stdcall Initialize() override;
            virtual HRESULT __stdcall GetCorSignature(_Out_ ISignatureBuilder** ppSignature) override;
            virtual HRESULT __stdcall CommitSignature() override;
            virtual HRESULT __stdcall GetCount(_Out_ DWORD* pdwCount) override;
            virtual HRESULT __stdcall AddLocal(_In_ IType* pType, _Out_opt_ DWORD* pIndex) override;
            virtual HRESULT __stdcall ReplaceSignature(_In_reads_bytes_(dwSigSize) const BYTE* pSignature, _In_ DWORD dwSigSize) override;

        // ILocalVariableCollection2
        public:
            virtual HRESULT __stdcall GetEnum(_Out_ IEnumMethodLocals** ppEnumMethodLocals) override;

        private:
            HRESULT GetOriginalSignatureInfo(
                _Out_ IEnumTypes** ppEnumTypes);
    };
}