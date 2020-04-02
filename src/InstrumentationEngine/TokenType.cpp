// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "TokenType.h"

HRESULT MicrosoftInstrumentationEngine::CTokenType::GetToken(_Out_ mdToken * token)
{
    IfNullRetPointer(token);
    *token = m_token;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CTokenType::GetOwningModule(_Out_ IModuleInfo** ppOwningModule)
{
    IfNullRetPointer(ppOwningModule);
    HRESULT hr = m_pOwningModule.CopyTo(ppOwningModule);
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CTokenType::AddToSignature(_In_ ISignatureBuilder* pSignatureBuilder)
{
    IfNullRetPointer(pSignatureBuilder);

    HRESULT hr = S_OK;
    IfFailRet(CType::AddToSignature(pSignatureBuilder));
    IfFailRet(pSignatureBuilder->AddToken(m_token));
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CTokenType::GetName(_Out_ BSTR* pbstrName)
{
    IfNullRetPointer(pbstrName);

    HRESULT hr = S_OK;
    if (m_name.Length() == 0)
    {
        CComPtr<IMetaDataImport> pImport;
        IfFailRet(m_pOwningModule->GetMetaDataImport((IUnknown **)&pImport));

        if (TypeFromToken(m_token) == mdtTypeDef)
        {
            mdTypeDef tkEnclosing = mdTokenNil;
            mdTypeDef tkCurrent = m_token;
            tstring fullName;
            vector<WCHAR> nameBuffer(100);
            ULONG cchLength = 0;
            while ((pImport->GetNestedClassProps(tkCurrent, &tkEnclosing) == S_OK) && (tkEnclosing != mdTokenNil))
            {
                IfFailRet(pImport->GetTypeDefProps(tkEnclosing, nullptr, 0, &cchLength, nullptr, nullptr));
                if (nameBuffer.size() < cchLength)
                {
                    nameBuffer.resize(cchLength);
                }

                // Add nested class names separated by a "/".
                IfFailRet(pImport->GetTypeDefProps(tkEnclosing, nameBuffer.data(), cchLength, &cchLength, nullptr, nullptr));
                fullName.insert(0, _T("/"));
                fullName.insert(0, nameBuffer.data());

                tkCurrent = tkEnclosing;
            }

            IfFailRet(pImport->GetTypeDefProps(m_token, nullptr, 0, &cchLength, nullptr, nullptr));
            if (nameBuffer.size() < cchLength)
            {
                nameBuffer.resize(cchLength);
            }

            IfFailRet(pImport->GetTypeDefProps(m_token, nameBuffer.data(), cchLength, &cchLength, nullptr, nullptr));
            fullName += nameBuffer.data();

            m_name = CComBSTR(static_cast<ULONG>(fullName.length()+1), fullName.c_str());
        }
        else if (TypeFromToken(m_token) == mdtTypeRef)
        {
            // Note: there are no public apis to get the outer classes for a nested
            // type ref. We don't want to attempt to resolve the type ref though, because
            // that could cause a module load at a time when such an operation is not
            // allowed.
            ULONG cchLength = 0;
            IfFailRet(pImport->GetTypeRefProps(m_token, nullptr, nullptr, 0, &cchLength));
            std::vector<WCHAR> nameBuffer(cchLength);
            IfFailRet(pImport->GetTypeRefProps(m_token, nullptr, nameBuffer.data(), cchLength, &cchLength));
            m_name = CComBSTR(cchLength, nameBuffer.data());
        }

    }
    if (m_name.Length() > 0)
    {
        return m_name.CopyTo(pbstrName);
    }
    return E_FAIL;
}

MicrosoftInstrumentationEngine::CTokenType::CTokenType(_In_ IModuleInfo* module, _In_ mdToken token, _In_ CorElementType type) : CType(type), m_token(token), m_pOwningModule(module)
{
    DEFINE_REFCOUNT_NAME(CTokenType);
}