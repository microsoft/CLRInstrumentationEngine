// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "NativeInstanceMethodInstrumentationInfo.h"
#include "MethodInstrumentationInfoKey.h"

namespace Agent
{
namespace Instrumentation
{
	CMethodInstrumentationInfoKey::CMethodInstrumentationInfoKey(
		CMethodInstrumentationInfoKey&& val) noexcept
		: m_argsCount(val.GetArgsCount())
	{
		m_strAssemblyName.swap(val.m_strAssemblyName);
		m_strModuleName.swap(val.m_strModuleName);
		m_strMethodName.swap(val.m_strMethodName);
	}

	CMethodInstrumentationInfoKey::CMethodInstrumentationInfoKey(
		const Native::CNativeInstanceMethodInstrumentationInfo& val)
		: m_strAssemblyName(val.GetAssemblyName())
		, m_strModuleName(val.GetModuleName())
		, m_strMethodName(val.GetMethodName())
		, m_argsCount(val.GetArgsCount())
	{
	}

	CMethodInstrumentationInfoKey::CMethodInstrumentationInfoKey(
		const std::wstring& strAssemblyName,
		const std::wstring& strModuleName,
		const std::wstring& strMethodName,
		const int			argsCount)
		: m_strAssemblyName(strAssemblyName)
		, m_strModuleName(strModuleName)
		, m_strMethodName(strMethodName)
		, m_argsCount(argsCount)
	{
	}

	CMethodInstrumentationInfoKey::~CMethodInstrumentationInfoKey()
	{
	}

	const std::wstring& CMethodInstrumentationInfoKey::GetAssemblyName() const
	{
		return m_strAssemblyName;
	}

	const std::wstring& CMethodInstrumentationInfoKey::GetModuleName() const
	{
		return m_strModuleName;
	}

	const std::wstring& CMethodInstrumentationInfoKey::GetMethodName() const
	{
		return m_strMethodName;
	}

	int CMethodInstrumentationInfoKey::GetArgsCount() const
	{
		return m_argsCount;
	}

	bool operator<(const CMethodInstrumentationInfoKey& left, const CMethodInstrumentationInfoKey& right)
	{
		if (left.m_strAssemblyName < right.m_strAssemblyName)
		{
			return true;
		}

		if (left.m_strAssemblyName == right.m_strAssemblyName
			&& left.m_strModuleName < right.m_strModuleName)
		{
			return true;
		}

		if (left.m_strModuleName == right.m_strModuleName
			&& left.m_strMethodName < right.m_strMethodName)
		{
			return true;
		}

		if (left.m_strMethodName == right.m_strMethodName
			&& left.m_argsCount < right.m_argsCount)
		{
			return true;
		}

		return false;
	}
}
}