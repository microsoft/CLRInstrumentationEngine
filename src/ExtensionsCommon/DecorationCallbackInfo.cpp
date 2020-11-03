// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "DecorationCallbackInfo.h"
#include "WellKnownInstrumentationDefs.h"

CDecorationCallbackInfo::CDecorationCallbackInfo(
	const std::wstring& strMethodName,
	DWORD dwAttributes,
    _In_reads_(ulCorSigSize) PCCOR_SIGNATURE pCorSig,
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

DWORD CDecorationCallbackInfo::GetParamCount() const
{
	const auto InvalidArgumentCount = -1;

	if (m_vecMethodSig.size() >= MinimalSigSize)
	{
		return m_vecMethodSig[ArgsCountSigIndex];
	}

	return InvalidArgumentCount;
}