// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "InstrumentationEngineDefs.h"
#include "InstrumentationEnginePointerDefs.h"
#include "MethodRecord.h"

struct ICodeInjector
{
	virtual HRESULT Inject(
		_In_ const IMethodInfoSptr& sptrMethodInfo,
        _In_ const CMethodRecordSptr& spMethodRecord) = 0;

	virtual HRESULT EmitModule(
		_In_ const IModuleInfoSptr& sptrModuleInfo) = 0;

	virtual ~ICodeInjector() {}

protected:
	ICodeInjector() noexcept {}
	ICodeInjector(const ICodeInjector&) = delete;
};