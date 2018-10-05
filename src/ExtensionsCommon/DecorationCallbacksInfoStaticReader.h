// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "DecorationCallbacksInfo.h"
#include "ProfilerHostServices.h"

class CDecorationCallbacksInfoStaticReader final : public CProfilerHostServices
{
public:
	_Success_(return == 0)
	HRESULT Read(
		_In_ const IModuleInfoSptr& sptrModuleInfo,
		_Out_ CDecorationCallbacksInfoSptr& spCallbacksInfo);

	virtual ~CDecorationCallbacksInfoStaticReader() {}
private:
	HRESULT InternalGenerateCallbackInfosForNCustomArgs(
		const std::wstring strMehodName,
		const unsigned int uMaxCustomArgsCount,
		PCCOR_SIGNATURE	pCorSigTemplate,
		unsigned int	ucCorSigTemplateSize,
		_Out_ DecorationCallbackInfoCollection& vecCallbacksResult);
}; // CDecorationCallbacksInfoStaticReader

typedef std::shared_ptr<CDecorationCallbacksInfoStaticReader> CDecorationCallbacksInfoStaticReaderSptr;