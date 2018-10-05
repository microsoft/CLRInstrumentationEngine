// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"

#include "DecorationCallbackInfo.h"
#include "WellKnownInstrumentationDefs.h"

CDecorationCallbackInfo::CDecorationCallbackInfo(
	const std::wstring& strMethodName,
	DWORD dwAttributes,
	PCCOR_SIGNATURE pCorSig,
	ULONG ulCorSigSize,
	DWORD dwImplFlags)
	: m_strMethodName(strMethodName)
	, m_dwMethodAttributes(dwAttributes)
	, m_vecMethodSig(pCorSig, pCorSig + ulCorSigSize)
	, m_dwMethodImplFlags(dwImplFlags)
{
}

CDecorationCallbackInfo::~CDecorationCallbackInfo() {}

const std::wstring& CDecorationCallbackInfo::GetMethodName() const
{
	return m_strMethodName;
}

const CorSigDefinition& CDecorationCallbackInfo::GetMethodSig() const
{
	return m_vecMethodSig;
}

DWORD CDecorationCallbackInfo::GetMethodAttributes() const
{
	return m_dwMethodAttributes;
}

DWORD CDecorationCallbackInfo::GetMethodImplFlags() const
{
	return m_dwMethodImplFlags;
}

DWORD CDecorationCallbackInfo::GetParamCount()
{
	const auto InvalidArgumentCount = -1;

	if (m_vecMethodSig.size() >= MinimalSigSize)
	{
		return m_vecMethodSig[ArgsCountSigIndex];
	}

	return InvalidArgumentCount;
}