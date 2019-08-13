// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

typedef std::vector<COR_SIGNATURE> CorSigDefinition;

class CDecorationCallbackInfo final
{
	const std::wstring		m_strMethodName;
	const CorSigDefinition	m_vecMethodSig;
	const DWORD				m_dwMethodAttributes;
	const DWORD				m_dwMethodImplFlags;
public:
	CDecorationCallbackInfo(
		const std::wstring& strMethodName,
		DWORD dwAttributes,
		PCCOR_SIGNATURE pCorSig,
		ULONG ulCorSigSize,
		DWORD dwImplFlags);

	virtual ~CDecorationCallbackInfo();

	const std::wstring& GetMethodName() const;
	const CorSigDefinition& GetMethodSig() const;
	DWORD GetMethodAttributes() const;
	DWORD GetMethodImplFlags() const;
	DWORD GetParamCount();
};

typedef std::shared_ptr<CDecorationCallbackInfo> CDecorationCallbackInfoSptr;