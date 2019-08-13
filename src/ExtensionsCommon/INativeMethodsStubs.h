// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"

struct INativeMethodsStubs
{
	virtual UINT_PTR GetStubByName(const std::wstring& functionName) = 0;

	virtual ~INativeMethodsStubs(){}
};

typedef std::shared_ptr<INativeMethodsStubs> INativeMethodsStubsSptr;