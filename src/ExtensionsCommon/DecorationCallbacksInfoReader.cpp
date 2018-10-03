// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "DecorationCallbacksInfoReader.h"
#include "DecorationCallbackInfo.h"

#include "WellKnownInstrumentationDefs.h"

CDecorationCallbacksInfoReader::~CDecorationCallbacksInfoReader()
{
}

_Success_(return == 0)
HRESULT CDecorationCallbacksInfoReader::Read(
	_In_ const IModuleInfoSptr& spModuleInfo,
	_Out_ CDecorationCallbacksInfoSptr& spCallbacksInfo)
{
	IfTrueRet(nullptr == spModuleInfo, E_INVALIDARG);

	ATL::CComBSTR bstrModuleName;
	IfFailRet(spModuleInfo->GetModuleName(&bstrModuleName));

    if (bstrModuleName == ExtensionsBaseAssemblyName)
	{
		IMetaDataAssemblyImportSptr spIMetaDataAssemblyImport;
		IfFailRet(spModuleInfo->GetMetaDataAssemblyImport(
			reinterpret_cast<IUnknown**>(&spIMetaDataAssemblyImport)));

		PublicKeyContainer vecPublicKeyContainer;
		ASSEMBLYMETADATA asmMetadata = { 0 };
		DWORD dwAssemblyFlags = 0;
		IfFailRet(
			InternalReadAssemblyInfo(
				spIMetaDataAssemblyImport,
				vecPublicKeyContainer,
				asmMetadata,
				dwAssemblyFlags));

		IMetaDataImportSptr spIMetaDataImport;
		IfFailRet(spModuleInfo->GetMetaDataImport(
			reinterpret_cast<IUnknown**>(&spIMetaDataImport)));
		mdTypeDef tkTypeDef = mdTypeDefNil;
		IfFailRet(
			spIMetaDataImport->FindTypeDefByName(
                ExtensionsBasePublicContractType,
				mdTokenNil,
				&tkTypeDef));

		DecorationCallbackInfoCollection vecOnBeginCallbacks;
		IfFailRet(
			this->InternalReadCallbackMethodInfos(
				spIMetaDataImport,
				tkTypeDef,
                ExtensionsBaseFacadeTypeOnBeginMethodName,
				vecOnBeginCallbacks));

		DecorationCallbackInfoCollection vecOnEndCallbacks;
		IfFailRet(
			this->InternalReadCallbackMethodInfos(
				spIMetaDataImport,
				tkTypeDef,
                ExtensionsBaseFacadeTypeOnEndMethodName,
				vecOnEndCallbacks));

		DecorationCallbackInfoCollection vecOnExceptionCallbacks;
		IfFailRet(
			this->InternalReadCallbackMethodInfos(
				spIMetaDataImport,
				tkTypeDef,
                ExtensionsBaseFacadeTypeOnExceptionMethodName,
				vecOnExceptionCallbacks));

		IfFailRet(
			CreateAndBuildUp(
				spCallbacksInfo,
                ExtensionsBaseAssemblyName,
				vecPublicKeyContainer,
				asmMetadata,
				dwAssemblyFlags,
                ExtensionsBaseModuleName,
                ExtensionsBasePublicContractType,
				vecOnBeginCallbacks,
				vecOnEndCallbacks,
				vecOnExceptionCallbacks));

		return S_OK;
	}

	return S_FALSE;
}

_Success_(return == 0)
HRESULT CDecorationCallbacksInfoReader::InternalReadAssemblyInfo(
	_In_ const IMetaDataAssemblyImportSptr& spIMetaDataAssemblyImport,
	_Out_ PublicKeyContainer& vecPublicKey,
	_Out_ ASSEMBLYMETADATA& asmMetadata,
	_Out_ DWORD& dwAssemblyFlags)
{
	LPCVOID			pbPublicKey = nullptr;
	ULONG			cbPublicKey = 0;
	ULONG			chName = MAX_PATH;
	LPCVOID			pbHashValue = nullptr;
	ULONG			cbHashValue = 0;
	ULONG			ulHashAlgId = 0;
	std::wstring	strName(chName, wchar_t());

	mdAssembly tkAssembly = mdAssemblyNil;
	IfFailRet(spIMetaDataAssemblyImport->GetAssemblyFromScope(&tkAssembly));

	IfFailRet(spIMetaDataAssemblyImport->GetAssemblyProps(
		tkAssembly,
		&pbPublicKey,
		&cbPublicKey,
		&ulHashAlgId,
		&strName[0],
		static_cast<ULONG>(strName.size()),
		&chName,
		&asmMetadata,
		&dwAssemblyFlags));

	PublicKeyContainer vecPublicKeyLocal(
		reinterpret_cast<LPCBYTE>(pbPublicKey),
		reinterpret_cast<LPCBYTE>(pbPublicKey)+cbPublicKey);

	vecPublicKey.swap(vecPublicKeyLocal);

	return S_OK;
}

_Success_(return == 0)
HRESULT CDecorationCallbacksInfoReader::InternalReadCallbackMethodInfos(
	_In_ const IMetaDataImportSptr& spIMetadataImport,
	_In_ const mdTypeDef	mdType,
	_In_ LPCWSTR szMethodName,
	_Out_ DecorationCallbackInfoCollection& vecCallbacksResult)
{
	const auto MaxOverloadedMethodsCount = 100;

	// reading method token of defined Begin callbacks
	std::vector<mdMethodDef> vecPrefixMethodTokens(
		MaxOverloadedMethodsCount,
		mdTokenNil);

	ULONG ulReceivedCount = 0;
	HCORENUM hEnum = nullptr;
	IfFailRet(spIMetadataImport->EnumMethodsWithName(
		&hEnum,
		mdType,
		szMethodName,
		&vecPrefixMethodTokens[0],
		static_cast<ULONG>(vecPrefixMethodTokens.size()),
		&ulReceivedCount));
	spIMetadataImport->CloseEnum(hEnum); // IfFailTrace
	vecPrefixMethodTokens.resize(ulReceivedCount);

	DecorationCallbackInfoCollection vecCallbacks;
	vecCallbacks.reserve(ulReceivedCount);

	for (auto tkMethodDef : vecPrefixMethodTokens)
	{
		PCCOR_SIGNATURE pCorSig = nullptr;
		ULONG ulCorSigSize = 0;
		DWORD dwAttributes = 0;
		DWORD dwImplFlags = 0;

		IfFailRet(
			spIMetadataImport->GetMethodProps(
				tkMethodDef,
				nullptr,
				nullptr,
				0,
				nullptr,
				&dwAttributes,
				&pCorSig,
				&ulCorSigSize,
				nullptr,
				&dwImplFlags));

		DecorationCallbackInfoCollection::value_type spCallbackInfo;
		IfFailRet(
			Create(
				spCallbackInfo,
				szMethodName,
				dwAttributes,
				pCorSig,
				ulCorSigSize,
				dwImplFlags));

		vecCallbacks.push_back(spCallbackInfo);
	}

	vecCallbacksResult.swap(vecCallbacks);

	return S_OK;
}