// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "NativeInstanceMethodInstrumentationInfo.h"
#include "TextUtils.h"

namespace Agent
{
namespace Instrumentation
{
namespace Native
{
	CNativeInstanceMethodInstrumentationInfo::CNativeInstanceMethodInstrumentationInfo(
		const std::wstring& strAssemblyName,
		const std::wstring& strModuleName,
		const std::wstring& strMethodName,
		const int			argsCount,
		const int			methodId,
		const UINT_PTR		pInstance,
		const UINT_PTR		pInstanceMethod)
		: m_strAssemblyName(strAssemblyName)
		, m_strModuleName(strModuleName)
		, m_strMethodName(strMethodName)
		, m_argsCount(argsCount)
		, m_methodId(methodId)
		, m_pInstance(pInstance)
		, m_pInstanceMethod(pInstanceMethod)
	{
		IfTrueThrow(Text::TextUtils::IsEmpty(strAssemblyName), E_INVALIDARG);
		IfTrueThrow(Text::TextUtils::IsEmpty(strModuleName), E_INVALIDARG);
		IfTrueThrow(Text::TextUtils::IsEmpty(strMethodName), E_INVALIDARG);
		IfTrueThrow(argsCount < 0, E_INVALIDARG);
		IfTrueThrow(0 == pInstance, E_INVALIDARG);
		IfTrueThrow(0 == pInstanceMethod, E_INVALIDARG);
	}

	CNativeInstanceMethodInstrumentationInfo::~CNativeInstanceMethodInstrumentationInfo()
	{
	}

	const std::wstring& CNativeInstanceMethodInstrumentationInfo::GetAssemblyName() const
	{
		return m_strAssemblyName;
	}

	const std::wstring& CNativeInstanceMethodInstrumentationInfo::GetModuleName() const
	{
		return m_strModuleName;
	}

	const std::wstring& CNativeInstanceMethodInstrumentationInfo::GetMethodName() const
	{
		return m_strMethodName;
	}

	int CNativeInstanceMethodInstrumentationInfo::GetArgsCount() const
	{
		return m_argsCount;
	}

	int CNativeInstanceMethodInstrumentationInfo::GetMethodId() const
	{
		return m_methodId;
	}

	UINT_PTR CNativeInstanceMethodInstrumentationInfo::GetInstancePtr() const
	{
		return m_pInstance;
	}

	UINT_PTR CNativeInstanceMethodInstrumentationInfo::GetInstanceMethodPtr() const
	{
		return m_pInstanceMethod;
	}
} // Native
} // Instrumentation
} // Agent