// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ProfilerHostServices.h"
#include "WellKnownTypeInfo.h"

typedef std::unordered_map<CorElementType, std::shared_ptr<Agent::Reflection::CWellKnownTypeInfo>> TStandardTypesList;

class CLoadArgumentsHelper
	: public CProfilerHostServices
{
private:
    TStandardTypesList m_typeMap;

public:
	CLoadArgumentsHelper();
	virtual ~CLoadArgumentsHelper();

	HRESULT BoxRetValueIfNeeded(
		_In_ const IMethodInfoSptr& sptrMethodInfo,
		_Inout_ IInstructionSptr& sptrCurrent);

	HRESULT UnBoxRetValueIfNeeded(
		_In_ const IMethodInfoSptr& sptrMethodInfo,
		_Inout_ IInstructionSptr& sptrCurrentInstruction);

    HRESULT LoadArgumentsAsObjects(
        _In_ const IMethodInfoSptr& sptrMethodInfo,
        _Inout_ IInstructionSptr& sptrCurrent);

private:

    HRESULT InternalLoadThisAsObject(
		_In_ const IMethodInfoSptr& sptrMethodInfo,
		_Inout_ IInstructionSptr& sptrCurrent);

    HRESULT InternalLoadInstanceArgumentsAsObjects(
		_In_ const IMethodInfoSptr& sptrMethodInfo,
		_Inout_ IInstructionSptr& sptrCurrent);

    HRESULT InternalLoadStaticArgumentsAsObjects(
		_In_ const IMethodInfoSptr& sptrMethodInfo,
		_Inout_ IInstructionSptr& sptrCurrent);

	HRESULT CLoadArgumentsHelper::GetBoxToken(
		_In_ const IMethodInfoSptr& spMethodInfo,
		_In_ const ITypeSptr& spType,
		_Out_ bool& bIsValueType,
		_Out_ mdToken& tkValueTypeToken
		);

	HRESULT InternalLoadArgumentsAsObjects(
		USHORT usFirstParamIndex,
		_In_ const IMethodInfoSptr& sptrMethodInfo,
		_Inout_ IInstructionSptr& sptrCurrent);
};