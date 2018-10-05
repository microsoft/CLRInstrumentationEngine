// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "DecorationCallbackInfo.h"
#include "ProfilerHostServices.h"

typedef std::vector<CDecorationCallbackInfoSptr> DecorationCallbackInfoCollection;
typedef std::vector<BYTE> PublicKeyContainer;

class CDecorationCallbacksInfo final : public CProfilerHostServices
{
	const std::wstring						m_strAssemblyName;
	const std::wstring						m_strModuleName;
	const std::wstring						m_strTypeName;

	const DecorationCallbackInfoCollection	m_vecBeginCallbacks;
	const DecorationCallbackInfoCollection	m_vecEndCallbacks;
	const DecorationCallbackInfoCollection	m_vecExceptionCallbacks;

	const ASSEMBLYMETADATA					m_asmMetaData;
	const PublicKeyContainer				m_vecPublicKey;
	const DWORD								m_dwAssemblyFlags;
public:
	CDecorationCallbacksInfo(
		const std::wstring& strAssemblyName,
		const PublicKeyContainer& vecPublicKey,
		const ASSEMBLYMETADATA& asmMetadata,
		const DWORD				dwAssemblyFlags,
		const std::wstring&		strModuleName,
		const std::wstring&		strTypeName,
		const DecorationCallbackInfoCollection& vecBeginCallbacks,
		const DecorationCallbackInfoCollection& vecEndCallbacks,
		const DecorationCallbackInfoCollection& vecExceptionCallbacks);

	virtual ~CDecorationCallbacksInfo();

	const std::wstring& GetAssemblyName() const;
	const ASSEMBLYMETADATA& GetAssemblyMetadata() const;
	const DWORD GetAssemblyFlags() const;

	const std::wstring& GetModuleName() const;
	const std::wstring GetTypeName() const;
	const DecorationCallbackInfoCollection& GetBeginCallbacks() const;
	const DecorationCallbackInfoCollection& GetEndCallbacks() const;
	const DecorationCallbackInfoCollection& GetExceptionCallbacks() const;
	const PublicKeyContainer& GetPublicKey() const;
}; // CDecorationCallbacksInfo

typedef std::shared_ptr<CDecorationCallbacksInfo> CDecorationCallbacksInfoSptr;