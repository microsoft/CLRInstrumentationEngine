// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "NativeInstanceMethodInstrumentationInfo.h"

namespace Agent
{
namespace Instrumentation
{
	class CMethodInstrumentationInfoKey final
	{
		std::wstring	m_strAssemblyName;
		std::wstring	m_strModuleName;
		std::wstring	m_strMethodName;
		const int		m_argsCount;

		AGENT_DECLARE_NO_LOGGING;
		friend bool operator<(
			const CMethodInstrumentationInfoKey& left,
			const CMethodInstrumentationInfoKey& right);
	public:
		CMethodInstrumentationInfoKey(
			CMethodInstrumentationInfoKey&& val) noexcept;

		CMethodInstrumentationInfoKey(
			const Native::CNativeInstanceMethodInstrumentationInfo& val);

		CMethodInstrumentationInfoKey(
			const std::wstring& strAssemblyName,
			const std::wstring& strModuleName,
			const std::wstring& strMethodName,
			const int			argsCount);

		virtual ~CMethodInstrumentationInfoKey();

		const std::wstring& GetAssemblyName() const;
		const std::wstring& GetModuleName() const;
		const std::wstring& GetMethodName() const;
		int GetArgsCount() const;
	}; // CMethodInstrumentationInfoKey

	bool operator<(const CMethodInstrumentationInfoKey& left, const CMethodInstrumentationInfoKey& right);
}
}