// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "SigParser.h"
#include "SignatureBuilder.h"

namespace MicrosoftInstrumentationEngine
{
    const int MAX_NAME = 1000;
    const int MAX_SUPPORTED_GENERIC_CONSTRAINTS = 1000;
    const WCHAR MERGE_TYPE_PREFIX = L'_';

    class AssemblyInjector final
    {
    public:
        AssemblyInjector(_In_ ICorProfilerInfo2* pProfilerInfo,
            _In_ IMetaDataImport2* pSourceImport,
            _In_ const LPCBYTE* pSourceImageBaseAddress,
            _In_ IMetaDataImport2* pTargetImport,
            _In_ IMetaDataEmit2* pTargetEmit,
            _In_ const ModuleID pTargetImage,
            _In_ IMethodMalloc* pTargetMethodMalloc);

        HRESULT ImportAll(bool importCustomAttributes = false);

    private:
        HRESULT ImportTypeDef(_In_ const mdTypeDef sourceTypeDef, _Out_ mdTypeDef *pTargetTypeDef);
        HRESULT ImportTypeRef(_In_ const mdTypeRef sourceTypeRef, _Out_ mdToken *pTargetTypeRef);
        HRESULT ImportAssemblyRef(_In_ const mdAssemblyRef tkTypeRef, _Out_ mdToken *pTargetAssemblyRef);
        HRESULT ImportFieldDef(_In_ const mdFieldDef sourceFieldDef, _Out_ mdFieldDef *pTargetFieldDef);
        HRESULT ImportMethodDef(_In_ const mdMethodDef sourceMethodDef, _Out_ mdMethodDef *pTargetMethodDef);
        HRESULT ImportLocalVarSig(_In_ const mdSignature sourceLocalVarSig, _Out_ mdSignature *pTargetLocalVarSig);
        HRESULT ImportMemberRef(_In_ const mdSignature sourceMemberRef, _Out_ mdSignature *pTargetMemberRef);
        HRESULT ImportString(_In_ const mdString sourceString, _Out_ mdString *pTargetString);
        HRESULT ImportProperty(_In_ const mdProperty sourceProperty, _Out_ mdProperty *pTargetProperty);
        HRESULT ImportEvent(_In_ const mdEvent sourceEvent, _Out_ mdEvent *pTargetEvent);
        HRESULT ImportCustomAttribute(_In_ const mdCustomAttribute sourceCustomAttribute, _Out_ mdCustomAttribute *pTargetCustomAttribute);
        HRESULT ImportMethodImpl(_In_ const mdTypeDef sourceImplementationClass, _In_ mdToken sourceImplementationMethod, _In_ mdToken sourceDeclarationMethod);
        HRESULT ImportParam(_In_ const mdParamDef sourceParam, _Out_ mdParamDef* pTargetParam);
        HRESULT ImportModuleRef(_In_ const mdModuleRef sourceModuleRef, _Out_ mdModuleRef* pTargetModuleRef);
        HRESULT ImportTypeSpec(_In_ const mdTypeSpec sourceTypeSpec, _Out_ mdTypeSpec* pTargetTypeSpec);
        HRESULT ImportGenericParam(_In_ const mdGenericParam sourceGenericParam, _Out_ mdGenericParam* pTargetGenericParam);
        HRESULT ImportMethodSpec(_In_ const mdMethodSpec sourceMethodSpec, _Out_ mdMethodSpec* pTargetMethodSpec);

        HRESULT ConvertToken(_In_ mdToken token, _Out_ mdToken* pTargetToken);
        HRESULT ConvertILCode(_In_ const BYTE* pSourceILCode, _In_ BYTE* pTargetILCode, ULONG32 ilCodeSize);
        HRESULT ConvertNonTypeSignatureCached(_Inout_ PCCOR_SIGNATURE* pSignature, _Inout_ DWORD* pcbSignature);
        HRESULT ConvertTypeSignatureCached(_Inout_ PCCOR_SIGNATURE* pSignature, _Inout_ DWORD* pcbSignature);
        HRESULT ConvertNonTypeSignature(_In_ SigParser & sig, _In_ const CComPtr<ISignatureBuilder>& newSig);
        HRESULT ConvertMethodDefRefOrPropertySignature(_In_ SigParser & sig, _In_ const CComPtr<ISignatureBuilder>& newSig, _In_ BYTE callConv);
        HRESULT ConvertFieldSignature(_In_ SigParser & sig, _In_ const CComPtr<ISignatureBuilder>& newSig);
        HRESULT ConvertMethodSpecSignature(_In_ SigParser & sig, _In_ const CComPtr<ISignatureBuilder>& newSig);
        HRESULT ConvertParamSignature(_In_ SigParser & sig, _In_ const CComPtr<ISignatureBuilder>& newSig);
        HRESULT ConvertCustomModSignatureList(_In_ SigParser & sig, _In_ const CComPtr<ISignatureBuilder>& newSig);
        HRESULT ConvertLocalVarSignature(_In_ SigParser & sig, _In_ const CComPtr<ISignatureBuilder>& newSig);
        HRESULT ConvertTypeSignature(_In_ SigParser & sig, _In_ const CComPtr<ISignatureBuilder>& newSig);
        HRESULT ConvertCustomModSignature(_In_ SigParser & sig, _In_ const CComPtr<ISignatureBuilder>& newSig);
        HRESULT ConvertToken(_In_ SigParser & sig, _In_ const CComPtr<ISignatureBuilder>& newSig);
        HRESULT ConvertData(_In_ SigParser & sig, _In_ const CComPtr<ISignatureBuilder>& newSig);
        HRESULT ConvertElemType(_In_ SigParser & sig, _In_ const CComPtr<ISignatureBuilder>& newSig);
        HRESULT ConvertLengthPrefixedDataList(_In_ SigParser & sig, _In_ const CComPtr<ISignatureBuilder>& newSig);

        typedef std::unordered_map<PCCOR_SIGNATURE, CComPtr<ISignatureBuilder>> TSignatureMap;
        TSignatureMap m_sigToConvertedSigMap;

        typedef std::unordered_map<mdToken, mdToken> TokenMap;
        TokenMap m_typeDefMap;
        TokenMap m_typeRefMap;
        TokenMap m_assemblyRefMap;
        TokenMap m_fieldDefMap;
        TokenMap m_methodDefMap;
        TokenMap m_memberRefMap;
        TokenMap m_stringMap;
        TokenMap m_propertyMap;
        TokenMap m_eventMap;
        TokenMap m_customAttributeMap;
        TokenMap m_moduleRefMap;
        TokenMap m_typeSpecMap;
        TokenMap m_genericParamMap;
        TokenMap m_methodSpecMap;

        ATL::CComPtr<IMetaDataImport2> m_pSourceImport;
        ATL::CComPtr<IMetaDataImport2> m_pTargetImport;
        ATL::CComPtr<IMetaDataEmit2> m_pTargetEmit;
        ATL::CComPtr<IMethodMalloc> m_pTargetMethodMalloc;
        ATL::CComPtr<ICorProfilerInfo2> m_pProfilerInfo;
        const LPCBYTE* m_pSourceImageBaseAddress;
        const ModuleID m_pTargetImage;
    };

}