// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "Type.h"
#include "SimpleType.h"
#include "SignatureBuilder.h"

namespace MicrosoftInstrumentationEngine
{
    //Temporary helper
    const WCHAR* GetNameFromCorType(_In_ CorElementType corType)
    {
        switch (corType)
        {
        case ELEMENT_TYPE_END:
            return _T("END");
        case ELEMENT_TYPE_VOID:
            return _T("VOID");
        case ELEMENT_TYPE_BOOLEAN:
            return _T("BOOLEAN");
        case ELEMENT_TYPE_CHAR:
            return _T("CHAR");
        case ELEMENT_TYPE_I1:
            return _T("I1");
        case ELEMENT_TYPE_U1:
            return _T("U1");
        case ELEMENT_TYPE_I2:
            return _T("I2");
        case ELEMENT_TYPE_U2:
            return _T("U2");
        case ELEMENT_TYPE_I4:
            return _T("I4");
        case ELEMENT_TYPE_U4:
            return _T("U4");
        case ELEMENT_TYPE_I8:
            return _T("I8");
        case ELEMENT_TYPE_U8:
            return _T("U8");
        case ELEMENT_TYPE_R4:
            return _T("R4");
        case ELEMENT_TYPE_R8:
            return _T("R8");
        case ELEMENT_TYPE_STRING:
            return _T("STRING");
        case ELEMENT_TYPE_PTR:
            return _T("PTR");
        case ELEMENT_TYPE_BYREF:
            return _T("BYREF");
        case ELEMENT_TYPE_VALUETYPE:
            return _T("VALUETYPE");
        case ELEMENT_TYPE_CLASS:
            return _T("CLASS");
        case ELEMENT_TYPE_VAR:
            return _T("VAR");
        case ELEMENT_TYPE_ARRAY:
            return _T("ARRAY");
        case ELEMENT_TYPE_GENERICINST:
            return _T("GENERICINST");
        case ELEMENT_TYPE_TYPEDBYREF:
            return _T("TYPEDBYREF");
        case ELEMENT_TYPE_I:
            return _T("I");
        case ELEMENT_TYPE_U:
            return _T("U");
        case ELEMENT_TYPE_FNPTR:
            return _T("FNPTR");
        case ELEMENT_TYPE_OBJECT:
            return _T("OBJECT");
        case ELEMENT_TYPE_SZARRAY:
            return _T("SZARRAY");
        case ELEMENT_TYPE_MVAR:
            return _T("MVAR");
        case ELEMENT_TYPE_CMOD_REQD:
            return _T("CMOD_REQD");
        case ELEMENT_TYPE_CMOD_OPT:
            return _T("CMOD_OPT");
        case ELEMENT_TYPE_INTERNAL:
            return _T("INTERNAL");
        case ELEMENT_TYPE_MAX:
            return _T("MAX");
        case ELEMENT_TYPE_MODIFIER:
            return _T("MODIFIER");
        case ELEMENT_TYPE_SENTINEL:
            return _T("SENTINEL");
        case ELEMENT_TYPE_PINNED:
            return _T("PINNED");
        default:
            return _T("INVALID");
        }
    }
}

MicrosoftInstrumentationEngine::CType::CType(
    _In_ CorElementType type
    )
    : m_type(type),
      m_isPinned(false),
      m_isSentinel(false),
      m_modifiers()
{
    DEFINE_REFCOUNT_NAME(CType);
}

HRESULT MicrosoftInstrumentationEngine::CType::GetCorElementType(_Out_ CorElementType* pCorType)
{
    IfNullRetPointer(pCorType);
    *pCorType = m_type;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CType::AddToSignature(_In_ ISignatureBuilder* pSigBuilder)
{
    IfNullRetPointer(pSigBuilder);
    HRESULT hr = S_OK;

    if (m_isSentinel)
    {
        IfFailRet(pSigBuilder->AddElementType(ELEMENT_TYPE_SENTINEL));
    }
    if (m_isPinned)
    {
        IfFailRet(pSigBuilder->AddElementType(ELEMENT_TYPE_PINNED));
    }

    for (IType* modifier : m_modifiers)
    {
        IfFailRet(modifier->AddToSignature(pSigBuilder));
    }

    IfFailRet(pSigBuilder->AddElementType(m_type));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CType::IsPrimitive(_Out_ BOOL* pIsPrimitive)
{
    IfNullRetPointer(pIsPrimitive);
    *pIsPrimitive = (m_type <= ELEMENT_TYPE_STRING) || (m_type == ELEMENT_TYPE_I) || (m_type == ELEMENT_TYPE_U);
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CType::IsArray(_Out_ BOOL* pIsArray)
{
    IfNullRetPointer(pIsArray);
    *pIsArray = (m_type == ELEMENT_TYPE_ARRAY) || (m_type == ELEMENT_TYPE_SZARRAY);
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CType::GetName(_Out_ BSTR* pbstrName)
{
    IfNullRetPointer(pbstrName);
    CComBSTR bstrName = MicrosoftInstrumentationEngine::GetNameFromCorType(m_type);
    *pbstrName = bstrName.Detach();
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CType::IsClass(_Out_ BOOL* pIsClass)
{
    *pIsClass = FALSE;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CType::SetIsPinned(_In_ bool isPinned)
{
    m_isPinned = isPinned;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CType::SetIsSentinel(_In_ bool isSentinel)
{
    m_isSentinel = isSentinel;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CType::SetModifers(_In_ const std::vector<CComPtr<IType>> & modifiers)
{
    m_modifiers = modifiers;
    return S_OK;
}