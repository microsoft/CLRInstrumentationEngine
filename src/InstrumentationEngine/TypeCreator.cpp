// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "TypeCreator.h"
#include "SimpleType.h"
#include <cor.h>
#include "GenericInstanceType.h"
#include "ArrayType.h"
#include "GenericParameterType.h"
#include "CompositeType.h"
#include "TokenType.h"
#include "ModifierType.h"
#include "FunctionType.h"
#include "EnumeratorImpl.h"

MicrosoftInstrumentationEngine::CTypeCreator::CTypeCreator(_In_ IModuleInfo* pModuleInfo) : m_pModuleInfo(pModuleInfo)
{
    DEFINE_REFCOUNT_NAME(CTypeCreator);
}

HRESULT MicrosoftInstrumentationEngine::CTypeCreator::FromCorElement(_In_ CorElementType type, _Out_ IType ** ppType)
{
    IfNullRetPointer(ppType);
    *ppType = nullptr;

    CLogging::LogMessage(_T("Starting CTypeCreator::FromCorElement"));

    switch (type)
    {
        case ELEMENT_TYPE_VOID:
        case ELEMENT_TYPE_BOOLEAN:
        case ELEMENT_TYPE_CHAR:
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
        case ELEMENT_TYPE_I4:
        case ELEMENT_TYPE_U4:
        case ELEMENT_TYPE_I8:
        case ELEMENT_TYPE_U8:
        case ELEMENT_TYPE_R4:
        case ELEMENT_TYPE_R8:
        case ELEMENT_TYPE_I:
        case ELEMENT_TYPE_U:
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_OBJECT:
        {
            CComPtr<IType> simpleType;
            simpleType.Attach(new CSimpleType(type));
            if (simpleType == nullptr)
            {
                return E_OUTOFMEMORY;
            }
            *ppType = simpleType.Detach();
            return S_OK;
        }
    }

    CLogging::LogMessage(_T("End CTypeCreator::FromCorElement"));

    return E_NOTIMPL;
}
HRESULT MicrosoftInstrumentationEngine::CTypeCreator::FromSignature(_In_ DWORD cbBuffer, _In_ const BYTE* pCorSignature, _Out_ IType ** ppType, _Out_opt_ DWORD* pdwSigSize)
{
    CLogging::LogMessage(_T("Starting CTypeCreator::FromSignature"));

    IfNullRetPointer(pCorSignature);
    IfNullRetPointer(pdwSigSize);
    IfNullRetPointer(ppType);
    if (cbBuffer == 0)
    {
        return E_INVALIDARG;
    }

    *ppType = nullptr;
    *pdwSigSize = 0;

    HRESULT hr = S_OK;
    PCCOR_SIGNATURE currentSignature = pCorSignature;
    CorElementType sigElement = ELEMENT_TYPE_END;
    DWORD currentSize = 1;

    CComPtr<IType> createdType;

    switch ((sigElement = static_cast<CorElementType>(*currentSignature)))
    {
        case ELEMENT_TYPE_VOID:
        case ELEMENT_TYPE_BOOLEAN:
        case ELEMENT_TYPE_CHAR:
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
        case ELEMENT_TYPE_I4:
        case ELEMENT_TYPE_U4:
        case ELEMENT_TYPE_I8:
        case ELEMENT_TYPE_U8:
        case ELEMENT_TYPE_R4:
        case ELEMENT_TYPE_R8:
        case ELEMENT_TYPE_I:
        case ELEMENT_TYPE_U:
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_OBJECT:
        case ELEMENT_TYPE_TYPEDBYREF:
        {
            createdType.Attach(new CSimpleType(sigElement));
            if (createdType == nullptr)
            {
                return E_OUTOFMEMORY;
            }
        }
        break;
    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_VALUETYPE:
        {
            mdToken tokenValue;
            currentSize += CorSigUncompressToken(&currentSignature[currentSize], &tokenValue);
            IfFailRet(FromToken(sigElement, tokenValue, &createdType));
        }
        break;
    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_BYREF:
    case ELEMENT_TYPE_SZARRAY:
        {
            DWORD newSize;
            CComPtr<IType> relatedType;
            IfFailRet(FromSignature(cbBuffer - currentSize, &currentSignature[currentSize], &relatedType, &newSize));
            createdType.Attach(new CCompositeType(sigElement, relatedType));
            if (createdType == nullptr)
            {
                return E_OUTOFMEMORY;
            }
            currentSize += newSize;
        }
        break;
    case ELEMENT_TYPE_FNPTR:
        {
            CComPtr<IType> pReturnType;
            ULONG convention = IMAGE_CEE_CS_CALLCONV_MAX;
            CComPtr<IEnumTypes> pEnumTypes;
            DWORD genericCount = 0;

            DWORD newSize = 0;
            IfFailRet(ParseMethodSignature(
                &currentSignature[currentSize],
                cbBuffer - currentSize,
                &convention,
                &pReturnType,
                &pEnumTypes,
                &genericCount,
                &newSize));
            currentSize += newSize;

            vector<CComPtr<IType>> parameterTypes;

            ULONG cTypes;
            CComPtr<IType> pType;
            while (S_OK == (hr = pEnumTypes->Next(1, &pType, &cTypes)))
            {
                parameterTypes.push_back(pType);

                pType.Release();
            }
            IfFailRet(hr);
            hr = S_OK;

            createdType.Attach(new CFunctionType((CorCallingConvention)convention, pReturnType, parameterTypes, genericCount));
            if (createdType == nullptr)
            {
                return E_OUTOFMEMORY;
            }
        }
        break;
    case ELEMENT_TYPE_ARRAY:
        {
            DWORD newSize;
            CComPtr<IType> relatedType;
            IfFailRet(FromSignature(cbBuffer - currentSize, &currentSignature[currentSize], &relatedType, &newSize));
            currentSize += newSize;
            IfFailRet(cbBuffer > currentSize ? S_OK : E_UNEXPECTED);
            ULONG rank;
            ULONG sizeCount;
            ULONG boundsCount;

            currentSize += CorSigUncompressData(&currentSignature[currentSize], &rank);
            currentSize += CorSigUncompressData(&currentSignature[currentSize], &sizeCount);
            std::vector<ULONG> counts(sizeCount);

            for (ULONG iSizeIndex = 0; iSizeIndex < sizeCount; iSizeIndex++)
            {
                ULONG count;
                currentSize += CorSigUncompressData(&currentSignature[currentSize], &count);
                counts[iSizeIndex] = count;
            }

            currentSize += CorSigUncompressData(&currentSignature[currentSize], &boundsCount);
            std::vector<ULONG> bounds(boundsCount);

            for (ULONG iBoundIndex = 0; iBoundIndex < boundsCount; iBoundIndex++)
            {
                ULONG bound;
                currentSize += CorSigUncompressData(&currentSignature[currentSize], &bound);
                bounds[iBoundIndex] = bound;
            }

            createdType.Attach(new CArrayType(relatedType, rank, counts, bounds));
            if (createdType == nullptr)
            {
                return E_OUTOFMEMORY;
            }
        }
        break;
    case ELEMENT_TYPE_MVAR:
    case ELEMENT_TYPE_VAR:
         {
            ULONG position;
            currentSize += CorSigUncompressData(&currentSignature[currentSize], &position);

            createdType.Attach(new CGenericParameterType(sigElement, position));
            if (createdType == nullptr)
            {
                return E_OUTOFMEMORY;
            }
        }
        break;
    case ELEMENT_TYPE_GENERICINST:
        {
            DWORD newSize;
            CComPtr<IType> relatedType;
            IfFailRet(FromSignature(cbBuffer - currentSize, &currentSignature[currentSize], &relatedType, &newSize));
            currentSize += newSize;
            IfFailRet(cbBuffer > currentSize ? S_OK : E_UNEXPECTED);
            ULONG argumentCount = 0;
            currentSize += CorSigUncompressData(&currentSignature[currentSize], &argumentCount);
            std::vector<IType*> parameters;
            for (ULONG i = 0; i < argumentCount; i++)
            {
                IType* parameterType = nullptr;
                IfFailRet(FromSignature(cbBuffer - currentSize, &currentSignature[currentSize], &parameterType, &newSize));
                currentSize += newSize;
                IfFailRet(cbBuffer >= currentSize ? S_OK : E_UNEXPECTED);
                parameters.push_back(parameterType);
            }

            createdType.Attach(new CGenericInstance(relatedType, parameters));
            for (IType* parameter : parameters)
            {
                parameter->Release();
            }
            if (createdType == nullptr)
            {
                return E_OUTOFMEMORY;
            }
        }
        break;
    case ELEMENT_TYPE_PINNED:
        {
            DWORD newSize = 0;
            IfFailRet(FromSignature(cbBuffer - currentSize, &currentSignature[currentSize], &createdType, &newSize));
            static_cast<CType*>(createdType.p)->SetIsPinned(true);
            currentSize += newSize;
        }
        break;
    case ELEMENT_TYPE_SENTINEL:
        {
            DWORD newSize = 0;
            IfFailRet(FromSignature(cbBuffer - currentSize, &currentSignature[currentSize], &createdType, &newSize));
            static_cast<CType*>(createdType.p)->SetIsSentinel(true);
            currentSize += newSize;
        }
        break;
    case ELEMENT_TYPE_CMOD_REQD:
    case ELEMENT_TYPE_CMOD_OPT:
        {
            mdToken token = mdTokenNil;
            currentSize += CorSigUncompressToken(&currentSignature[currentSize], &token);
            std::vector<CComPtr<IType>> modifiers;

            CComPtr<IType> pModifierType;
            pModifierType.Attach(new CModifierType(sigElement, token));
            if (pModifierType == nullptr)
            {
                return E_OUTOFMEMORY;
            }

            modifiers.push_back(pModifierType);

            CorElementType modifierElementType = (CorElementType)currentSignature[currentSize];
            while ((modifierElementType == ELEMENT_TYPE_CMOD_REQD) || (modifierElementType == ELEMENT_TYPE_CMOD_OPT))
            {
                ++currentSize;
                mdToken tokenInner = mdTokenNil;
                currentSize += CorSigUncompressToken(&currentSignature[currentSize], &tokenInner);
                CComPtr<IType> pNextModifierType;
                pNextModifierType.Attach(new CModifierType(modifierElementType, tokenInner));
                if (pNextModifierType == nullptr)
                {
                    return E_OUTOFMEMORY;
                }
                modifiers.push_back(pNextModifierType);
                modifierElementType = (CorElementType)currentSignature[currentSize];
            }

            DWORD newSize = 0;
            IfFailRet(FromSignature(cbBuffer - currentSize, &currentSignature[currentSize], &createdType, &newSize));
            currentSize += newSize;

            static_cast<CType*>(createdType.p)->SetModifers(modifiers);
        }
        break;
    default:
        CLogging::LogError(_T("Unexpected element type %d. This usually indicates a signature parsing bug"), sigElement);
        return E_NOTIMPL;
    }

    if (createdType)
    {
        *ppType = createdType.Detach();
        *pdwSigSize = currentSize;
        hr = S_OK;
    }
    else
    {
        CLogging::LogError(_T("Type %d is not yet supported"), sigElement);
        hr = E_FAIL;
    }

    CLogging::LogMessage(_T("End CTypeCreator::FromSignature"));
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CTypeCreator::FromToken(_In_ CorElementType type, _In_ mdToken token, _Out_ IType** ppType)
{
    IfNullRetPointer(ppType);
    if ((type != ELEMENT_TYPE_CLASS) && (type != ELEMENT_TYPE_VALUETYPE))
    {
        return E_INVALIDARG;
    }

    *ppType = nullptr;

    HRESULT hr = S_OK;

    CComPtr<IType> tokenType;
    tokenType.Attach(new CTokenType(m_pModuleInfo, token, type));
    if (tokenType == nullptr)
    {
        return E_OUTOFMEMORY;
    }
    *ppType = tokenType.Detach();
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CTypeCreator::IsValueType(_In_ mdTypeDef mdTypeDefToken, _Out_ BOOL* pIsValueType)
{
    IfNullRetPointer(pIsValueType);

    HRESULT hr = S_OK;
    *pIsValueType = FALSE;

    CComPtr<IMetaDataImport> pMetadataImport;
    IfFailRet(m_pModuleInfo->GetMetaDataImport((IUnknown**)&pMetadataImport));

    mdToken tkBaseType = mdTokenNil;
    DWORD flags = 0;
    IfFailRet(pMetadataImport->GetTypeDefProps(mdTypeDefToken, nullptr, 0, nullptr, &flags, &tkBaseType));

    if (IsTdInterface(flags) || IsTdAbstract(flags) || (!IsTdSealed(flags)) ||
        ((TypeFromToken(tkBaseType) != mdtTypeDef) && (TypeFromToken(tkBaseType) != mdtTypeRef)))
    {
        return S_OK;
    }

    CComPtr<IType> pBaseType;
    IfFailRet(FromToken(ELEMENT_TYPE_CLASS, tkBaseType, &pBaseType));
    CComBSTR baseName;
    IfFailRet(pBaseType->GetName(&baseName));

#ifndef PLATFORM_UNIX
    if ((wcscmp(_T("System.ValueType"), baseName) == 0) || (wcscmp(_T("System.Enum"), baseName) == 0))
#else
    if ((PAL_wcscmp(_T("System.ValueType"), baseName) == 0) || (PAL_wcscmp(_T("System.Enum"), baseName) == 0))
#endif
    {
        *pIsValueType = TRUE;
    }
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CTypeCreator::ParseMethodSignature(
    _In_ const BYTE* pSignature,
    _In_ DWORD cbSignature,
    _Out_opt_ ULONG* pCallingConvention,
    _Out_opt_ IType** ppReturnType,
    _Out_opt_ IEnumTypes** ppEnumParameterTypes,
    _Out_opt_ ULONG* pcGenericTypeParameters,
    _Out_opt_ ULONG* pcbRead
    )
{
    IfNullRetPointer(pSignature);

    if (pCallingConvention)
    {
        *pCallingConvention = IMAGE_CEE_CS_CALLCONV_MAX;
    }
    if (ppReturnType)
    {
        *ppReturnType = nullptr;
    }
    if (ppEnumParameterTypes)
    {
        *ppEnumParameterTypes = nullptr;
    }
    if (pcGenericTypeParameters)
    {
        *pcGenericTypeParameters = 0;
    }
    if (pcbRead)
    {
        *pcbRead = 0;
    }

    HRESULT hr = S_OK;

    ULONG cbRead = 0;

    // Read the calling convention
    ULONG callingConvention = IMAGE_CEE_CS_CALLCONV_MAX;
    cbRead = CorSigUncompressData(pSignature, &callingConvention);
    if ((callingConvention == IMAGE_CEE_CS_CALLCONV_MAX) ||
        (callingConvention == IMAGE_CEE_CS_CALLCONV_FIELD) ||
        (callingConvention == IMAGE_CEE_CS_CALLCONV_GENERICINST) ||
        (callingConvention == IMAGE_CEE_CS_CALLCONV_LOCAL_SIG) ||
        (callingConvention == IMAGE_CEE_CS_CALLCONV_PROPERTY))
    {
        CLogging::LogError(_T("Unexpected calling convention on method signature."));
        return E_UNEXPECTED;
    }
    IfFailRet(cbSignature > cbRead ? S_OK : E_UNEXPECTED);

    // Read number of generic type parameters
    ULONG cGenericTypeParameters = 0;
    if ((callingConvention & IMAGE_CEE_CS_CALLCONV_GENERIC) == IMAGE_CEE_CS_CALLCONV_GENERIC)
    {
        cbRead += CorSigUncompressData(pSignature + cbRead, &cGenericTypeParameters);
        IfFailRet(cbSignature > cbRead ? S_OK : E_UNEXPECTED);
    }

    // Read number of parameters
    ULONG cParameterTypes = 0;
    cbRead += CorSigUncompressData(pSignature + cbRead, &cParameterTypes);
    IfFailRet(cbSignature > cbRead ? S_OK : E_UNEXPECTED);

    ULONG cbReadType;

    // Read return type
    CComPtr<IType> pReturnType;
    IfFailRet(FromSignature(cbSignature - cbRead, pSignature + cbRead, &pReturnType, &cbReadType));
    cbRead += cbReadType;
    IfFailRet(cbSignature >= cbRead ? S_OK : E_UNEXPECTED);

    // Read parameter types
    CComPtr<IEnumTypes> pEnumParameterTypes;
    IfFailRet(ParseTypeSequence(pSignature + cbRead, cbSignature - cbRead, cParameterTypes, &pEnumParameterTypes, &cbReadType));
    cbRead += cbReadType;
    IfFailRet(cbSignature >= cbRead ? S_OK : E_UNEXPECTED);

    if (pCallingConvention)
    {
        *pCallingConvention = callingConvention;
    }
    if (ppReturnType)
    {
        *ppReturnType = pReturnType.Detach();
    }
    if (ppEnumParameterTypes)
    {
        *ppEnumParameterTypes = pEnumParameterTypes.Detach();
    }
    if (pcGenericTypeParameters)
    {
        *pcGenericTypeParameters = cGenericTypeParameters;
    }
    if (pcbRead)
    {
        *pcbRead = cbRead;
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CTypeCreator::ParseLocalVarSignature(
    _In_ const BYTE* pSignature,
    _In_ ULONG cbSignature,
    _Out_opt_ ULONG* pCallingConvention,
    _Out_opt_ IEnumTypes** ppEnumTypes,
    _Out_opt_ ULONG* pcbRead
    )
{
    IfNullRetPointer(pSignature);

    if (pCallingConvention)
    {
        *pCallingConvention = IMAGE_CEE_CS_CALLCONV_MAX;
    }
    if (ppEnumTypes)
    {
        *ppEnumTypes = nullptr;
    }
    if (pcbRead)
    {
        *pcbRead = 0;
    }

    HRESULT hr = S_OK;

    ULONG cbRead = 0;

    // Read the calling convention
    ULONG callingConvention = IMAGE_CEE_CS_CALLCONV_MAX;
    cbRead = CorSigUncompressData(pSignature, &callingConvention);
    if (callingConvention != IMAGE_CEE_CS_CALLCONV_LOCAL_SIG)
    {
        CLogging::LogError(_T("Unexpected calling convention on local variable signature."));
        return E_UNEXPECTED;
    }
    IfFailRet(cbSignature > cbRead ? S_OK : E_UNEXPECTED);

    // Read local variable count
    ULONG cLocals;
    cbRead += CorSigUncompressData(pSignature + cbRead, &cLocals);
    IfFailRet(cbSignature >= cbRead ? S_OK : E_UNEXPECTED);

    // Read local types
    ULONG cbReadTypes;
    CComPtr<IEnumTypes> pEnumTypes;
    IfFailRet(ParseTypeSequence(pSignature + cbRead, cbSignature - cbRead, cLocals, &pEnumTypes, &cbReadTypes));
    cbRead += cbReadTypes;
    IfFailRet(cbSignature >= cbRead ? S_OK : E_UNEXPECTED);

    if (pCallingConvention)
    {
        *pCallingConvention = callingConvention;
    }
    if (ppEnumTypes)
    {
        *ppEnumTypes = pEnumTypes.Detach();
    }
    if (pcbRead)
    {
        *pcbRead = cbRead;
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CTypeCreator::ParseTypeSequence(
    _In_ const BYTE* pBuffer,
    _In_ ULONG cbBuffer,
    _In_ ULONG cTypes,
    _Out_opt_ IEnumTypes** ppEnumTypes,
    _Out_opt_ ULONG* pcbRead
    )
{
    IfNullRetPointer(pBuffer);

    if (ppEnumTypes)
    {
        *ppEnumTypes = nullptr;
    }
    if (pcbRead)
    {
        *pcbRead = 0;
    }

    HRESULT hr = S_OK;

    ULONG cbRead = 0;
    ULONG cbReadType;

    vector<CComPtr<IType>> types;
    for (ULONG index = 0; index < cTypes; index++)
    {
        CComPtr<IType> pType;
        IfFailRet(FromSignature(cbBuffer - cbRead, pBuffer + cbRead, &pType, &cbReadType));
        cbRead += cbReadType;
        IfFailRet(cbBuffer >= cbRead ? S_OK : E_UNEXPECTED);

        types.push_back(pType);
    }

    if (ppEnumTypes)
    {
        CComPtr<CEnumerator<IEnumTypes, IType>> pEnumTypes;
        pEnumTypes.Attach(new CEnumerator<IEnumTypes, IType>());
        if (nullptr == pEnumTypes)
        {
            return E_OUTOFMEMORY;
        }
        IfFailRet(pEnumTypes->Initialize(types));

        *ppEnumTypes = pEnumTypes.Detach();
    }
    if (pcbRead)
    {
        *pcbRead = cbRead;
    }

    return S_OK;
}