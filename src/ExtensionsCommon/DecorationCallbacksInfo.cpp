// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"

#include "DecorationCallbacksInfo.h"

CDecorationCallbacksInfo::CDecorationCallbacksInfo(
	const std::wstring&			strAssemblyName,
	const PublicKeyContainer&	vecPublicKey,
	const ASSEMBLYMETADATA&		asmMetadata,
	const DWORD					dwAssemblyFlags,
	const std::wstring&			strModuleName,
	const std::wstring&			strTypeName,
	const DecorationCallbackInfoCollection& vecBeginCallbacks,
	const DecorationCallbackInfoCollection& vecEndCallbacks,
	const DecorationCallbackInfoCollection& vecExceptionCallbacks)
	: m_strAssemblyName(strAssemblyName)
	, m_strModuleName(strModuleName)
	, m_strTypeName(strTypeName)
	, m_vecBeginCallbacks(vecBeginCallbacks)
	, m_vecEndCallbacks(vecEndCallbacks)
	, m_vecExceptionCallbacks(vecExceptionCallbacks)
	, m_asmMetaData(asmMetadata)
	, m_dwAssemblyFlags(dwAssemblyFlags)
	, m_vecPublicKey(vecPublicKey)
{
	IfTrueThrow(0 == strAssemblyName.size(), E_INVALIDARG);
	IfTrueThrow(0 == strModuleName.size(), E_INVALIDARG);
	IfTrueThrow(0 == strTypeName.size(), E_INVALIDARG);
}

CDecorationCallbacksInfo::~CDecorationCallbacksInfo(){}

const std::wstring& CDecorationCallbacksInfo::GetAssemblyName() const
{
	return m_strAssemblyName;
}

const ASSEMBLYMETADATA& CDecorationCallbacksInfo::GetAssemblyMetadata() const
{
	return m_asmMetaData;
}

const DWORD CDecorationCallbacksInfo::GetAssemblyFlags() const
{
	return m_dwAssemblyFlags;
}

const std::wstring& CDecorationCallbacksInfo::GetModuleName() const
{
	return m_strModuleName;
}

const std::wstring CDecorationCallbacksInfo::GetTypeName() const
{
	return m_strTypeName;
}

const DecorationCallbackInfoCollection& CDecorationCallbacksInfo::GetBeginCallbacks() const
{
	return m_vecBeginCallbacks;
}

const DecorationCallbackInfoCollection& CDecorationCallbacksInfo::GetEndCallbacks() const
{
	return m_vecEndCallbacks;
}

const DecorationCallbackInfoCollection& CDecorationCallbacksInfo::GetExceptionCallbacks() const
{
	return m_vecExceptionCallbacks;
}

const PublicKeyContainer& CDecorationCallbacksInfo::GetPublicKey() const
{
	return m_vecPublicKey;
}