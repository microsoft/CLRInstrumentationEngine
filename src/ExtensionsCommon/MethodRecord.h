// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "ICodeInjectorFwd.h"

class CMethodRecord
{
public:
	const std::wstring m_assemblyName;
	const std::wstring m_moduleName;
	const std::wstring m_methodName;
	const int m_argsCount;
	const int m_methodId;
    const ICodeInjectorSPtr m_spCodeInjector;

	CMethodRecord(_In_ const std::wstring& assemblyName,
		_In_ const std::wstring& moduleName,
		_In_ const std::wstring& methodName,
		const int argsCount,
		const int methodId,
		_In_ const ICodeInjectorSPtr& spCodeInjector) :
		m_assemblyName(assemblyName),
		m_moduleName(moduleName),
		m_methodName(methodName),
		m_argsCount(argsCount),
		m_methodId(methodId),
        m_spCodeInjector(spCodeInjector)
	{
	}

	virtual ~CMethodRecord()
	{
	}
};

typedef std::shared_ptr<CMethodRecord> CMethodRecordSptr;