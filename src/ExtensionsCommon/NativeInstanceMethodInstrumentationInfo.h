// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "NativeInstanceMethodInstrumentationInfo.h"

namespace Agent
{
namespace Instrumentation
{
namespace Native
{
	class CNativeInstanceMethodInstrumentationInfo final
	{
		const std::wstring	m_strAssemblyName;
		const std::wstring	m_strModuleName;
		const std::wstring	m_strMethodName;
		const int			m_argsCount;
		const int			m_methodId;
		const UINT_PTR			m_pInstance;
		const UINT_PTR			m_pInstanceMethod;

		CNativeInstanceMethodInstrumentationInfo(const CNativeInstanceMethodInstrumentationInfo&) = delete;
		CNativeInstanceMethodInstrumentationInfo& operator=(const CNativeInstanceMethodInstrumentationInfo&) = delete;

	protected:
		AGENT_DECLARE_NO_LOGGING;


	public:
		CNativeInstanceMethodInstrumentationInfo(
			const std::wstring& strAssemblyName,
			const std::wstring& strModuleName,
			const std::wstring& strMethodName,
			const int			argsCount,
			const int			methodId,
			const UINT_PTR		pInstance,
			const UINT_PTR		pInstanceMethod);

		virtual ~CNativeInstanceMethodInstrumentationInfo();

		const std::wstring& GetAssemblyName() const;
		const std::wstring& GetModuleName() const;
		const std::wstring& GetMethodName() const;
		int GetArgsCount() const;
		int GetMethodId() const;

		UINT_PTR GetInstancePtr() const;
		UINT_PTR GetInstanceMethodPtr() const;
	}; // CNativeInstanceMethodInstrumentationInfo

	typedef std::shared_ptr<CNativeInstanceMethodInstrumentationInfo> CNativeInstanceMethodInstrumentationInfoSptr;

} // Native
} // Instrumentation
} // Agent