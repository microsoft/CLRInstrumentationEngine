// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "LocalVariableCollection.h"
#include "ProfilerManager.h"
#include "SignatureBuilder.h"
#include "EnumeratorImpl.h"
#include "MethodLocal.h"

MicrosoftInstrumentationEngine::CLocalVariableCollection::CLocalVariableCollection(_In_ IMethodInfo* pOwningMethod, _In_ bool bReadOnly)
: m_originalLocalsCount(0), m_initialized(false), m_pOwningMethod(pOwningMethod), m_bReadOnly(bReadOnly)
{
    DEFINE_REFCOUNT_NAME(CLocalVariableCollection);
}

HRESULT MicrosoftInstrumentationEngine::CLocalVariableCollection::Initialize()
{
    if (m_initialized)
    {
        return S_FALSE;
    }

    HRESULT hr = S_OK;

    CComPtr<IEnumTypes> pEnumLocalTypes;
    IfFailRet(GetOriginalSignatureInfo(&pEnumLocalTypes));
    if (hr == S_OK)
    {
        IfFailRet(pEnumLocalTypes->GetCount(&m_originalLocalsCount));
    }

    m_initialized = true;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CLocalVariableCollection::ReplaceSignature(_In_ const BYTE* pSignature, _In_ DWORD dwSigSize)
{
    //TODO Should we allow complete signature removal?

    CLogging::LogMessage(_T("Begin CLocalVariableCollection::ReplaceSignature"));

    if (m_bReadOnly)
    {
        CLogging::LogError(_T("Local variable collection is read-only"));
        return E_FAIL;
    }

    if (m_initialized)
    {
        CLogging::LogError(_T("Already initialized from original signature."));
        return E_FAIL;
    }
    IfNullRetPointer(pSignature);
    if (dwSigSize < 2)
    {
        return E_INVALIDARG;
    }
    if ((pSignature[0] & IMAGE_CEE_CS_CALLCONV_LOCAL_SIG) == 0)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    m_replacementSig.resize(dwSigSize);
    for (size_t i = 0; i < dwSigSize; i++)
    {
        m_replacementSig[i] = pSignature[i];
    }

    IfFailRet(Initialize());

    CLogging::LogMessage(_T("End CLocalVariableCollection::ReplaceSignature"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CLocalVariableCollection::GetCount(_Out_ DWORD* pdwCount)
{
    IfNullRetPointer(pdwCount);
    HRESULT hr = S_OK;
    IfFailRet(Initialize());
    *pdwCount = (DWORD)(m_originalLocalsCount + m_typesToAdd.size());
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CLocalVariableCollection::AddLocal(_In_ IType* pType, _Out_opt_ DWORD* pIndex)
{
    IfNullRetPointer(pType);

    HRESULT hr;
    IfFailRet(Initialize());

    if (m_bReadOnly)
    {
        CLogging::LogError(_T("Local variable collection is read-only"));
        return E_FAIL;
    }

    CComPtr<IType> typeToAdd(pType);
    m_typesToAdd.push_back(typeToAdd);
    if (pIndex)
    {
        *pIndex = (DWORD)(m_originalLocalsCount + m_typesToAdd.size() - 1);
    }
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CLocalVariableCollection::GetCorSignature(_Out_ ISignatureBuilder** ppSignature)
{
    IfNullRetPointer(ppSignature);
    *ppSignature = nullptr;

    HRESULT hr = S_OK;

    IfFailRet(Initialize());

    CComPtr<CSignatureBuilder> pSignature;
    pSignature.Attach(new CSignatureBuilder());
    if (pSignature == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    pSignature->Add(IMAGE_CEE_CS_CALLCONV_LOCAL_SIG);
    pSignature->Add(m_originalLocalsCount + (ULONG)m_typesToAdd.size());

    if (m_originalLocalsCount > 0)
    {
        CComPtr<IEnumTypes> pEnumLocalTypes;
        IfFailRet(GetOriginalSignatureInfo(&pEnumLocalTypes));
        if (hr == S_OK)
        {
            ULONG cTypes;
            CComPtr<IType> pType;
            while (S_OK == (hr = pEnumLocalTypes->Next(1, &pType, &cTypes)))
            {
                IfFailRet(pType->AddToSignature(pSignature));

                pType.Release();
            }
            IfFailRet(hr);
            hr = S_OK;
        }
    }

    for (IType* type : m_typesToAdd)
    {
        IfFailRet(type->AddToSignature(pSignature));
    }
    *ppSignature = pSignature.Detach();

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CLocalVariableCollection::CommitSignature()
{
    //No signature changes
    if ((!m_initialized) || (m_typesToAdd.empty() && m_replacementSig.empty()))
    {
        return S_FALSE;
    }

    if (m_bReadOnly)
    {
        CLogging::LogError(_T("Local variable collection is read-only"));
        return E_FAIL;
    }

    HRESULT hr;
    CComPtr<CProfilerManager> pProfilerManager;
    IfFailRet(CProfilerManager::GetProfilerManagerInstance(&pProfilerManager));

    CComPtr<ISignatureBuilder> signatureBuilder;
    IfFailRet(this->GetCorSignature(&signatureBuilder));

    CComPtr<IMetaDataEmit> emit;
    CComPtr<IModuleInfo> moduleInfo;
    IfFailRet(m_pOwningMethod->GetModuleInfo(&moduleInfo));
    IfFailRet(moduleInfo->GetMetaDataEmit((IUnknown **)&emit));
    DWORD size;
    const BYTE* signatureBuffer;
    IfFailRet(signatureBuilder->GetSize(&size));
    IfFailRet(signatureBuilder->GetCorSignaturePtr(&signatureBuffer));
    mdSignature newSigToken;
    IfFailRet(emit->GetTokenFromSig(signatureBuffer, size, &newSigToken));

    IfFailRet(m_pOwningMethod->SetLocalVarSigToken(newSigToken));

    m_typesToAdd.clear();
    m_originalLocalsCount = 0;
    m_initialized = false;
    m_replacementSig.clear();

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CLocalVariableCollection::GetEnum(
    _Out_ IEnumMethodLocals** ppEnumMethodLocals)
{
    IfNullRetPointer(ppEnumMethodLocals);

    *ppEnumMethodLocals = nullptr;

    HRESULT hr = S_OK;

    IfFailRet(Initialize());

    // Get the types for each local
    CComPtr<IEnumTypes> pEnumTypes;
    IfFailRet(GetOriginalSignatureInfo(&pEnumTypes));

    vector<CComPtr<IMethodLocal>> methodLocals;

    // Create IMethodLocal for each local type
    ULONG cTypes;
    CComPtr<IType> pType;
    while (S_OK == (hr = pEnumTypes->Next(1, &pType, &cTypes)))
    {
        CComPtr<IMethodLocal> pMethodLocal;
        pMethodLocal.Attach(new CMethodLocal(pType));
        if (nullptr == pMethodLocal)
        {
            return E_OUTOFMEMORY;
        }
        methodLocals.push_back(pMethodLocal);

        pType.Release();
    }
    IfFailRet(hr);
    hr = S_OK;

    // Create enumerator
    CComPtr<CEnumerator<IEnumMethodLocals, IMethodLocal>> pEnumLocals;
    pEnumLocals.Attach(new CEnumerator<IEnumMethodLocals, IMethodLocal>());
    if (pEnumLocals == nullptr)
    {
        return E_OUTOFMEMORY;
    }
    IfFailRet(pEnumLocals->Initialize(methodLocals));

    *ppEnumMethodLocals = pEnumLocals.Detach();
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CLocalVariableCollection::GetOriginalSignatureInfo(
    _Out_ IEnumTypes** ppEnumLocalTypes
    )
{
    IfNullRet(ppEnumLocalTypes);

    *ppEnumLocalTypes = nullptr;

    const BYTE* rgbOrigSig = nullptr;
    DWORD cbOrigSig = 0;
    HRESULT hr = S_OK;

    if (m_replacementSig.empty())
    {
        CComPtr<IModuleInfo> pModuleInfo;
        IfFailRet(m_pOwningMethod->GetModuleInfo(&pModuleInfo));

        CComPtr<IMetaDataImport> pMetadataImport;
        IfFailRet(pModuleInfo->GetMetaDataImport((IUnknown**)&pMetadataImport));

        mdToken tkLocalVarSig = 0;
        IfFailRet(m_pOwningMethod->GetLocalVarSigToken(&tkLocalVarSig));

        if (tkLocalVarSig != mdTokenNil)
        {
            IfFailRet(pMetadataImport->GetSigFromToken(tkLocalVarSig, &rgbOrigSig, &cbOrigSig));
        }
    }
    else
    {
        rgbOrigSig = m_replacementSig.data();
        cbOrigSig = (DWORD)m_replacementSig.size();
    }

    if ((rgbOrigSig != nullptr) && (cbOrigSig >= 2))
    {
        CComPtr<IModuleInfo> pModuleInfo;
        IfFailRet(m_pOwningMethod->GetModuleInfo(&pModuleInfo));
        CComPtr<ITypeCreator> pTypeCreator;
        IfFailRet(pModuleInfo->CreateTypeFactory(&pTypeCreator));
        CComQIPtr<ISignatureParser> pSignatureParser(pTypeCreator);
        IfNullRet(pSignatureParser);

        // Get the local types enumerator
        IfFailRet(pSignatureParser->ParseLocalVarSignature(
            rgbOrigSig,
            cbOrigSig,
            nullptr,
            ppEnumLocalTypes,
            nullptr));

        hr = S_OK;
    }
    else
    {
        //Original signature is empty

        // Create empty enumerator
        CComPtr<CEnumerator<IEnumTypes, IType>> pEnumLocals;
        pEnumLocals.Attach(new CEnumerator<IEnumTypes, IType>());
        if (pEnumLocals == nullptr)
        {
            return E_OUTOFMEMORY;
        }
        *ppEnumLocalTypes = pEnumLocals.Detach();

        hr = S_FALSE;
    }
    return hr;
}