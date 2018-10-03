// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ReflectionHelper.h"

_Success_(return == 0)
HRESULT CReflectionHelper::GetAssemblyRefByName(
    const IModuleInfoSptr& spModuleInfo,
    const std::wstring& strAssemblyName,
    _Out_ mdToken& tkResultAssemblyRef) const
{
    tkResultAssemblyRef = mdAssemblyRefNil;

    IMetaDataImportSptr spIMetaDataImport;
    IfFailRet(spModuleInfo->GetMetaDataImport(reinterpret_cast<IUnknown**>(&spIMetaDataImport)));

    IMetaDataAssemblyImportSptr spIMetaDataAssemblyImport;
    IfFailRet(spModuleInfo->GetMetaDataAssemblyImport(
        reinterpret_cast<IUnknown**>(&spIMetaDataAssemblyImport)));

    const auto MaxReferenceCount = 256;
    std::vector<mdAssemblyRef> vecAssemblyRefs(MaxReferenceCount, mdAssemblyRefNil);
    ULONG ulReceivedCount = 0;
    HCORENUM hEnum = nullptr;
    IfFailRet(spIMetaDataAssemblyImport->EnumAssemblyRefs(
        &hEnum,
        &vecAssemblyRefs[0],
        static_cast<ULONG>(vecAssemblyRefs.size()),
        &ulReceivedCount));
    spIMetaDataAssemblyImport->CloseEnum(hEnum); // IfFailTrace

    vecAssemblyRefs.resize(ulReceivedCount);

    for (mdAssemblyRef tkAssemblyRef : vecAssemblyRefs)
    {
        LPCVOID		pbPublicKey = nullptr;
        ULONG		cbPublicKey = 0;
        LPCVOID		pbHashValue = nullptr;
        ULONG		cbHashValue = 0;
        ULONG		chName = 0;

        std::wstring strName(MAX_PATH, wchar_t());
        DWORD dwAsemblyRefFlags = 0;
        ASSEMBLYMETADATA asmMetadata = { 0 };

        if (SUCCEEDED(spIMetaDataAssemblyImport->GetAssemblyRefProps(
            tkAssemblyRef,
            &pbPublicKey,
            &cbPublicKey,
            &strName[0],
            static_cast<ULONG>(strName.size()),
            &chName,
            &asmMetadata,
            &pbHashValue,
            &cbHashValue,
            &dwAsemblyRefFlags)))
        {
            strName.resize(0 == chName ? chName : chName - 1);

            if (0 == strName.compare(strAssemblyName))
            {
                tkResultAssemblyRef = tkAssemblyRef;

                return S_OK;
            }
        }
    }

    return S_FALSE;
}

HRESULT CReflectionHelper::IsTypeBelongsToModule(
    _In_ const IModuleInfoSptr& spModuleInfo,
    _In_ const std::shared_ptr<Agent::Reflection::CWellKnownTypeInfo> spWellKnownTypeInfo,
    _Out_ bool& bBelong)
{
    HRESULT hr = S_OK;
    bBelong = true;

    ATL::CComBSTR bstrModuleName;
    IfFailRet(spModuleInfo->GetModuleName(&bstrModuleName));
    if (bstrModuleName != spWellKnownTypeInfo->GetModuleName().c_str())
    {
        bBelong = false;
        return S_FALSE;
    }

    IAssemblyInfoSptr spAssemblyInfo;
    IfFailRet(spModuleInfo->GetAssemblyInfo(&spAssemblyInfo));
    ATL::CComBSTR bstrAssemblyName;
    spAssemblyInfo->GetName(&bstrAssemblyName);
    if (bstrAssemblyName != spWellKnownTypeInfo->GetAssemblyName().c_str())
    {
        bBelong = false;
        return S_FALSE;
    }

    return hr;
}

HRESULT CReflectionHelper::DefineMethodToken(
    _In_ const IModuleInfoSptr& spModuleInfo,
    _In_ const std::shared_ptr<Agent::Reflection::CWellKnownMethodInfo> spWellKnownMethodInfo,
    _Out_ mdToken& tkMethodToken)
{
    HRESULT hr = S_OK;

    IfFailRet(hr = GetOrDefineMethodToken(spModuleInfo, spWellKnownMethodInfo, tkMethodToken, true));

    return hr;
}


HRESULT CReflectionHelper::GetMethodToken(
    _In_ const IModuleInfoSptr& spModuleInfo,
    _In_ const std::shared_ptr<Agent::Reflection::CWellKnownMethodInfo> spWellKnownMethodInfo,
    _Out_ mdToken& tkMethodToken)
{
    HRESULT hr = S_OK;

    IfFailRet(hr = GetOrDefineMethodToken(spModuleInfo, spWellKnownMethodInfo, tkMethodToken, false));

    return hr;
}


HRESULT CReflectionHelper::GetOrDefineMethodToken(
    _In_ const IModuleInfoSptr& spModuleInfo,
    _In_ const std::shared_ptr<Agent::Reflection::CWellKnownMethodInfo> spWellKnownMethodInfo,
    _Out_ mdToken& tkMethodToken,
    _In_ bool bDefine)
{
    HRESULT hr = S_OK;
    tkMethodToken = mdTokenNil;

    mdToken tkTypeToken;
    IfFailRet(hr = GetOrDefineTypeToken(spModuleInfo, spWellKnownMethodInfo->GetType(), tkTypeToken, bDefine));

    if (hr == S_OK && TypeFromToken(tkTypeToken) == mdTypeDefNil)
    {
        IMetaDataImportSptr spIMetaDataImport;
        IfFailRet(spModuleInfo->GetMetaDataImport(
            reinterpret_cast<IUnknown**>(&spIMetaDataImport)));
        IfFailRet(
            hr = spIMetaDataImport->FindMethod(
            tkTypeToken,
            spWellKnownMethodInfo->GetMethodName().c_str(),
            &spWellKnownMethodInfo->GetMethodSig()[0],
            static_cast<ULONG>(spWellKnownMethodInfo->GetMethodSig().size()),
            &tkMethodToken));
    }
    else
    {
        IMetaDataImportSptr spIMetaDataImport;
        IfFailRet(spModuleInfo->GetMetaDataImport(
            reinterpret_cast<IUnknown**>(&spIMetaDataImport)));

        hr = spIMetaDataImport->FindMemberRef(
            tkTypeToken,
            spWellKnownMethodInfo->GetMethodName().c_str(),
            &spWellKnownMethodInfo->GetMethodSig()[0],
            static_cast<ULONG>(spWellKnownMethodInfo->GetMethodSig().size()),
            &tkMethodToken);

        if ((hr == CLDB_E_RECORD_NOTFOUND) && bDefine)
        {
            IMetaDataEmitSptr spIMetaDataEmit;
            IfFailRet(spModuleInfo->GetMetaDataEmit(
                reinterpret_cast<IUnknown**>(&spIMetaDataEmit)));

            IfFailRet(hr = spIMetaDataEmit->DefineMemberRef(tkTypeToken,
                spWellKnownMethodInfo->GetMethodName().c_str(),
                &spWellKnownMethodInfo->GetMethodSig()[0],
                static_cast<ULONG>(spWellKnownMethodInfo->GetMethodSig().size()),
                &tkMethodToken));
        }
        else if (FAILED(hr))
        {
            IfFailRet(hr);
        }
    }

    return hr;
}

HRESULT CReflectionHelper::GetTypeToken(
    _In_ const IModuleInfoSptr& spModuleInfo,
    _In_ const std::shared_ptr<Agent::Reflection::CWellKnownTypeInfo> spWellKnownTypeInfo,
    _Out_ mdToken& tkTypeToken)
{
    HRESULT hr = S_OK;

    IfFailRet(hr = GetOrDefineTypeToken(spModuleInfo, spWellKnownTypeInfo, tkTypeToken, false));

    return hr;
}

HRESULT CReflectionHelper::DefineTypeToken(
    _In_ const IModuleInfoSptr& spModuleInfo,
    _In_ const std::shared_ptr<Agent::Reflection::CWellKnownTypeInfo> spWellKnownTypeInfo,
    _Out_ mdToken& tkTypeToken)
{
    HRESULT hr = S_OK;

    IfFailRet(hr = GetOrDefineTypeToken(spModuleInfo, spWellKnownTypeInfo, tkTypeToken, true));

    return hr;
}

HRESULT CReflectionHelper::GetOrDefineTypeToken(
    _In_ const IModuleInfoSptr& spModuleInfo,
    _In_ const std::shared_ptr<Agent::Reflection::CWellKnownTypeInfo> spWellKnownTypeInfo,
    _Out_ mdToken& tkTypeToken,
    _In_ bool bDefine)
{
    HRESULT hr = S_OK;
    tkTypeToken = mdTokenNil;

    bool bBelong = false;
    IfFailRet(IsTypeBelongsToModule(spModuleInfo, spWellKnownTypeInfo, bBelong));
    if (bBelong)
    {
        IMetaDataImportSptr spIMetaDataImport;
        IfFailRet(spModuleInfo->GetMetaDataImport(
            reinterpret_cast<IUnknown**>(&spIMetaDataImport)));
        IfFailRet(hr = spIMetaDataImport->FindTypeDefByName(
            spWellKnownTypeInfo->GetTypeName().c_str(),
            mdTokenNil,
            &tkTypeToken));
    }
    else
    {
        mdToken tkAssemblyToken;
        IfFailRet(GetAssemblyRefByName(spModuleInfo, spWellKnownTypeInfo->GetAssemblyName(), tkAssemblyToken));

        IMetaDataImportSptr spIMetaDataImport;
        IfFailRet(spModuleInfo->GetMetaDataImport(
            reinterpret_cast<IUnknown**>(&spIMetaDataImport)));

        hr = spIMetaDataImport->FindTypeRef(tkAssemblyToken, spWellKnownTypeInfo->GetTypeName().c_str(), &tkTypeToken);
        if ((hr == CLDB_E_RECORD_NOTFOUND) && bDefine)
        {
            IMetaDataEmitSptr spIMetaDataEmit;
            IfFailRet(spModuleInfo->GetMetaDataEmit(
                reinterpret_cast<IUnknown**>(&spIMetaDataEmit)));

            IfFailRet(hr = spIMetaDataEmit->DefineTypeRefByName(tkAssemblyToken, spWellKnownTypeInfo->GetTypeName().c_str(), &tkTypeToken));
        }
        else if (FAILED(hr))
        {
            IfFailRet(hr);
        }
    }

    return hr;
}
