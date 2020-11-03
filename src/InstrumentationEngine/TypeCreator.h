// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "InstrumentationEngine.h"

#include "DataContainer.h"

using namespace MicrosoftInstrumentationEngine;

namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("C6D612FA-B550-48E3-8859-DE440CF66627"))
    CTypeCreator : public ITypeCreator, public ISignatureParser, public CDataContainer
    {
        friend class CModuleInfo;

        private:
            CComPtr<IModuleInfo> m_pModuleInfo;

        // ITypeCreator
        public:
            STDMETHOD(FromSignature)(_In_ DWORD cbBuffer, _In_ const BYTE* pCorSignature, _Out_ IType ** ppType, _Out_opt_ DWORD* pSigSize) override;
            STDMETHOD(FromCorElement)(_In_ CorElementType type, _Out_ IType** ppType) override;
            STDMETHOD(FromToken)(_In_ CorElementType type, _In_ mdToken token, _Out_ IType** ppType) override;

        // ISignatureParser
            STDMETHOD(ParseMethodSignature)(
                _In_reads_bytes_(cbSignature) const BYTE* pSignature,
                _In_ DWORD cbSignature,
                _Out_opt_ ULONG* pCallingConvention,
                _Out_opt_ IType** ppReturnType,
                _Out_opt_ IEnumTypes** ppEnumParameterTypes,
                _Out_opt_ ULONG* pcGenericTypeParameters,
                _Out_opt_ ULONG* pcbRead
                ) override;
            STDMETHOD(ParseLocalVarSignature)(
                _In_reads_bytes_(cbSignature) const BYTE* pSignature,
                _In_ ULONG cbSignature,
                _Out_opt_ ULONG* pCallingConvention,
                _Out_opt_ IEnumTypes** ppEnumTypes,
                _Out_opt_ ULONG* pcbRead
                ) override;
            STDMETHOD(ParseTypeSequence)(
                _In_reads_bytes_(cbBuffer) const BYTE* pBuffer,
                _In_ ULONG cbBuffer,
                _In_ ULONG cTypes,
                _Out_opt_ IEnumTypes** ppEnumTypes,
                _Out_opt_ ULONG* pcbRead
                ) override;
        public:
            DEFINE_DELEGATED_REFCOUNT_ADDREF(CTypeCreator);
            DEFINE_DELEGATED_REFCOUNT_RELEASE(CTypeCreator);
            STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
            {
                return ImplQueryInterface(
                    static_cast<ITypeCreator*>(this),
                    static_cast<ISignatureParser*>(this),
                    static_cast<IDataContainer*>(this),
                    riid,
                    ppvObject
                    );
            }
        public:
            HRESULT IsValueType(_In_ mdTypeDef mdTypeDefToken, _Out_ BOOL* isValueType);
        protected:
            CTypeCreator(_In_ IModuleInfo* pModuleInfo);
    };
}