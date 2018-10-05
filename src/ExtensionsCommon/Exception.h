// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

namespace Agent
{
namespace Diagnostics
{
	class CException
	{
		HRESULT m_code;
	public:
        CException(HRESULT code = S_FALSE) noexcept;
		CException(const CException&);
		CException& operator= (const CException&);

		virtual ~CException();

		HRESULT GetCode() const;
	}; // CException
}
}