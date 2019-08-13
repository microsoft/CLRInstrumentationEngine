// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "DecorationCallbacksInfo.h"
#include "ProfilerHostServices.h"

class CDecorationCallbacksInfoReader final : public CProfilerHostServices
{
public:
	virtual ~CDecorationCallbacksInfoReader();

	_Success_(return == 0)
	HRESULT Read(
		_In_ const IModuleInfoSptr& sptrModuleInfo,
		_Out_ CDecorationCallbacksInfoSptr& spCallbacksInfo);

private:
	_Success_(return == 0)
	HRESULT InternalReadCallbackMethodInfos(
		_In_ const IMetaDataImportSptr& sptrIMetadataImport,
		_In_ const mdTypeDef	mdType,
		_In_ LPCWSTR szMethodName,
		_Out_ DecorationCallbackInfoCollection& vecCallbacks);

	_Success_(return == 0)
	HRESULT InternalReadAssemblyInfo(
		_In_ const IMetaDataAssemblyImportSptr& spIMetaDataAssemblyImport,
		_Out_ PublicKeyContainer& vecPublicKey,
		_Out_ ASSEMBLYMETADATA& asmMetadata,
		_Out_ DWORD& dwAssemblyFlags);
};

typedef std::shared_ptr<CDecorationCallbacksInfoReader> CDecorationCallbacksInfoReaderSptr;