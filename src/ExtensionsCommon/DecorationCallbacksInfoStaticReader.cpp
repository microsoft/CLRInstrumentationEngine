// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "DecorationCallbacksInfoStaticReader.h"

#include "WellKnownInstrumentationDefs.h"

static const BYTE ExtensionBaseAssemblyPublicKey[] =
{
	0x00, 0x24, 0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x94, 0x00, 0x00, 0x00, 0x06, 0x02, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x52, 0x53, 0x41, 0x31, 0x00, 0x04, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x4d, 0xdb, 0x14, 0xfd, 0x25, 0xfa, 0x54, 0xef, 0x1f, 0xe0, 0x55, 0x16, 0xd6, 0x9c, 0x0b, 0xb1, 0x9c, 0x86, 0x95, 0x6e, 0x2d, 0x52, 0x45, 0xe7, 0x28, 0x30, 0x04, 0x17, 0xe6, 0xa0, 0x18, 0xda,
	0xac, 0x56, 0xb6, 0x1e, 0xe2, 0x15, 0xe4, 0xc0, 0x96, 0xdb, 0xa9, 0x42, 0x36, 0x8b, 0xb4, 0xaa, 0x76, 0x95, 0x60, 0x42, 0xbb, 0x3e, 0xfb, 0x70, 0x9c, 0xda, 0x84, 0x7d, 0x73, 0x96, 0x83, 0x9f,
	0x57, 0xa4, 0x0b, 0x90, 0x82, 0x9f, 0xe5, 0xf3, 0x47, 0xa5, 0xd2, 0xe2, 0xc1, 0x98, 0x36, 0x7c, 0xbc, 0x10, 0x92, 0xaa, 0x97, 0x62, 0xae, 0x97, 0x76, 0xe5, 0x9f, 0xed, 0x16, 0x70, 0x38, 0x87,
	0x32, 0x9f, 0xfe, 0xb6, 0xd6, 0xcb, 0xf4, 0x48, 0x53, 0xc4, 0x96, 0xa2, 0x2b, 0xc7, 0x9b, 0xb3, 0xce, 0x00, 0xf2, 0x97, 0x60, 0x99, 0x5d, 0xaf, 0xa6, 0xaa, 0x97, 0x77, 0x99, 0x83, 0xe0, 0xb4
};

static const COR_SIGNATURE BeginCallbackTheShortestSignature[] =
{
	IMAGE_CEE_CS_CALLCONV_DEFAULT, // call convention
	0x1, // args count
	ELEMENT_TYPE_OBJECT, // retval type
	ELEMENT_TYPE_I4 // int methodId
};

static const COR_SIGNATURE EndCallbackTheShortestSignature[] =
{
	IMAGE_CEE_CS_CALLCONV_DEFAULT, // call convention
	0x3, // args count
	ELEMENT_TYPE_OBJECT, // retval type
	ELEMENT_TYPE_I4, // int methodId
	ELEMENT_TYPE_OBJECT, // object context
	ELEMENT_TYPE_OBJECT, // object returnValue
};

static const COR_SIGNATURE ExceptionCallbackTheShortestSignature[] =
{
	IMAGE_CEE_CS_CALLCONV_DEFAULT, // call convention
	0x3, // args count
	ELEMENT_TYPE_VOID, // retval type
	ELEMENT_TYPE_I4, // int methodId
	ELEMENT_TYPE_OBJECT, // object context
	ELEMENT_TYPE_OBJECT, // object exception
};

HRESULT CDecorationCallbacksInfoStaticReader::InternalGenerateCallbackInfosForNCustomArgs(
	const std::wstring strMehodName,
	const unsigned int uMaxCustomArgsCount,
	PCCOR_SIGNATURE	pCorSigTemplate,
	unsigned int	ucCorSigTemplateSize,
	_Out_ DecorationCallbackInfoCollection& vecCallbacksResult)
{
	const DWORD MethodAttributes = 0;
	const DWORD MethodImplFlags = 0;

	DecorationCallbackInfoCollection vecCallbacks;
	vecCallbacks.reserve(static_cast<size_t>(uMaxCustomArgsCount) + 1);

	for (unsigned int idx = 0; idx <= uMaxCustomArgsCount; ++idx)
	{
		CorSigDefinition vecCorSignature(
			pCorSigTemplate,
			pCorSigTemplate + ucCorSigTemplateSize);
		vecCorSignature[ArgsCountSigIndex] += idx;

		vecCorSignature.reserve(static_cast<size_t>(ucCorSigTemplateSize) + idx);

		for (unsigned int argsIdx = 1; argsIdx <= idx; ++argsIdx)
		{
			vecCorSignature.push_back(ELEMENT_TYPE_OBJECT);
		}

		DecorationCallbackInfoCollection::value_type spCallbackInfo;
		IfFailRet(
			Create(
				spCallbackInfo,
				strMehodName,
				MethodAttributes,
				&vecCorSignature[0],
				static_cast<ULONG>(vecCorSignature.size()),
				MethodImplFlags));

		vecCallbacks.push_back(spCallbackInfo);
	}

	vecCallbacksResult.swap(vecCallbacks);

	return S_OK;
}

_Success_(return == 0)
HRESULT CDecorationCallbacksInfoStaticReader::Read(
	_In_ const IModuleInfoSptr& spModuleInfo,
	_Out_ CDecorationCallbacksInfoSptr& spCallbacksInfo)
{
	IfTrueRet(nullptr == spModuleInfo, E_INVALIDARG);

	ATL::CComBSTR bstrModuleName;
	IfFailRet(spModuleInfo->GetModuleName(&bstrModuleName));

	// reading at first non mscorlib module
	if (bstrModuleName != WellKnownMscorlibModuleName)
	{
		PublicKeyContainer vecPublicKey(
            ExtensionBaseAssemblyPublicKey,
            ExtensionBaseAssemblyPublicKey + _countof(ExtensionBaseAssemblyPublicKey));

		ASSEMBLYMETADATA asmMetadata = { 0 };
		const DWORD dwAssemblyFlags = 0x00000001;

		DecorationCallbackInfoCollection vecBeginCallbacks;
		IfFailRet(
			InternalGenerateCallbackInfosForNCustomArgs(
                ExtensionsBaseFacadeTypeOnBeginMethodName,
				MaxCustomArgsCount,
				BeginCallbackTheShortestSignature,
				_countof(BeginCallbackTheShortestSignature),
				vecBeginCallbacks));

		DecorationCallbackInfoCollection vecEndCallbacks;
		IfFailRet(
			InternalGenerateCallbackInfosForNCustomArgs(
                ExtensionsBaseFacadeTypeOnEndMethodName,
				MaxCustomArgsCount,
				EndCallbackTheShortestSignature,
				_countof(EndCallbackTheShortestSignature),
				vecEndCallbacks));

		DecorationCallbackInfoCollection vecExceptionCallbacks;
		IfFailRet(
			InternalGenerateCallbackInfosForNCustomArgs(
            ExtensionsBaseFacadeTypeOnExceptionMethodName,
			MaxCustomArgsCount,
			ExceptionCallbackTheShortestSignature,
			_countof(ExceptionCallbackTheShortestSignature),
			vecExceptionCallbacks));

		IfFailRet(
			CreateAndBuildUp(
				spCallbacksInfo,
                ExtensionsBaseAssemblyName,
				vecPublicKey,
				asmMetadata,
				dwAssemblyFlags,
                ExtensionsBaseModuleName,
                ExtensionsBasePublicContractType,
				vecBeginCallbacks,
				vecEndCallbacks,
				vecExceptionCallbacks));

		return S_OK;
	}

	return S_FALSE;
}