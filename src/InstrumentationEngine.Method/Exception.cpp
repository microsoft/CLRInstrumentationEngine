// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "Exception.h"

namespace Agent
{
namespace Diagnostics
{

CException::CException(HRESULT code) noexcept
	: m_code(code)
{
}

CException::CException(const CException& other) : m_code(other.m_code)
{
}

CException& CException::operator = (const CException& other)
{
    if (this != &other)
    {
        this->m_code = other.m_code;
    }

	return *this;
}

CException::~CException()
{
}

HRESULT CException::GetCode() const
{
	return m_code;
}

}
}