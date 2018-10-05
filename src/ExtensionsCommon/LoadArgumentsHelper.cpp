// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "LoadArgumentsHelper.h"
#include "InstructionDefineExtensions.h"
#include "ReflectionHelper.h"
#include <unordered_map>
#include "WellKnownInstrumentationDefs.h"

CLoadArgumentsHelper::CLoadArgumentsHelper()
    : m_typeMap 
    {
        {ELEMENT_TYPE_BOOLEAN, std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.Boolean")},
        {ELEMENT_TYPE_CHAR,    std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.Char")},
        {ELEMENT_TYPE_I1,      std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.SByte")},
        {ELEMENT_TYPE_U1,      std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.Byte")},
        {ELEMENT_TYPE_I2,      std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.Int16")},
        {ELEMENT_TYPE_U2,      std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.UInt16")},
        {ELEMENT_TYPE_I4,      std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.Int32")},
        {ELEMENT_TYPE_U4,      std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.Unt32")},
        {ELEMENT_TYPE_I8,      std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.Int64")},
        {ELEMENT_TYPE_U8,      std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.Unt64")},
        {ELEMENT_TYPE_R4,      std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.Single")},
        {ELEMENT_TYPE_R8,      std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.Double")},
        {ELEMENT_TYPE_I,       std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.IntPtr")},
        {ELEMENT_TYPE_U,       std::make_shared<Agent::Reflection::CWellKnownTypeInfo>(L"mscorlib", L"mscorlib.dll", L"System.UIntPtr")},
    }
{
}

CLoadArgumentsHelper::~CLoadArgumentsHelper()
{
}

HRESULT CLoadArgumentsHelper::LoadArgumentsAsObjects(
    _In_ const IMethodInfoSptr& spMethodInfo,
    _Inout_ IInstructionSptr& spCurrentInstruction)
{
    BOOL fIsStatic = FALSE;
    IfFailRet(spMethodInfo->GetIsStatic(&fIsStatic));

    if (TRUE == fIsStatic)
    {
        IfFailRet(
            InternalLoadStaticArgumentsAsObjects(
            spMethodInfo,
            spCurrentInstruction));
    }
    else
    {
        IfFailRet(
            InternalLoadInstanceArgumentsAsObjects(
            spMethodInfo,
            spCurrentInstruction));
    }

    return S_OK;
}

HRESULT CLoadArgumentsHelper::InternalLoadThisAsObject(
	_In_ const IMethodInfoSptr& spMethodInfo,
	_Inout_ IInstructionSptr& spCurrentInstruction)
{
	IInstructionFactorySptr spInstructionFactory;
	IfFailRet(spMethodInfo->GetInstructionFactory(&spInstructionFactory));

	IInstructionGraphSptr spInstructionGraph;
	IfFailRet(spMethodInfo->GetInstructions(&spInstructionGraph));

	IInstructionSptr spLoadThis;
	IfFailRet(
		spInstructionFactory->CreateLoadArgInstruction(
			0,
			&spLoadThis));
	IfFailRet(spInstructionGraph->InsertAfter(spCurrentInstruction, spLoadThis));

	spCurrentInstruction = spLoadThis;

	ITypeSptr spThisType;
	IfFailRet(spMethodInfo->GetDeclaringType(&spThisType));

	bool bIsValueType = false;
	mdToken tkRetVal = mdTokenNil;
	IfFailRet(GetBoxToken(spMethodInfo, spThisType, bIsValueType, tkRetVal));
	if (bIsValueType)
	{
		IInstructionSptr spBoxRetValParamenterInstruction;
		IfFailRet(
			spInstructionFactory->CreateTokenOperandInstruction(
			Cee_Box,
			tkRetVal,
			&spBoxRetValParamenterInstruction));
		IfFailRet(spInstructionGraph->InsertAfter(
			spCurrentInstruction,
			spBoxRetValParamenterInstruction));
		spCurrentInstruction = spBoxRetValParamenterInstruction;
	}

	return S_OK;
}

HRESULT CLoadArgumentsHelper::InternalLoadInstanceArgumentsAsObjects(
	_In_ const IMethodInfoSptr& spMethodInfo,
	_Inout_ IInstructionSptr& spCurrent)
{
	const auto StartingArgumentIndex = 1;
	IfFailRet(InternalLoadThisAsObject(spMethodInfo, spCurrent));
	IfFailRet(InternalLoadArgumentsAsObjects(StartingArgumentIndex, spMethodInfo, spCurrent));

	return S_OK;
}

HRESULT CLoadArgumentsHelper::InternalLoadStaticArgumentsAsObjects(
	_In_ const IMethodInfoSptr& spMethodInfo,
	_Inout_ IInstructionSptr& spCurrent)
{
	const auto StartingArgumentIndex = 0;
	IfFailRet(InternalLoadArgumentsAsObjects(StartingArgumentIndex, spMethodInfo, spCurrent));

	return S_OK;
}

HRESULT CLoadArgumentsHelper::InternalLoadArgumentsAsObjects(
	USHORT usParamIndex,
	_In_ const IMethodInfoSptr& spMethodInfo,
	_Inout_ IInstructionSptr& spCurrentInstruction)
{
	IInstructionFactorySptr spInstructionFactory;
	IfFailRet(spMethodInfo->GetInstructionFactory(&spInstructionFactory));

	IInstructionGraphSptr spInstructionGraph;
	IfFailRet(spMethodInfo->GetInstructions(&spInstructionGraph));

	IEnumMethodParametersSptr spMethodParameters;
	IfFailRet(spMethodInfo->GetParameters(&spMethodParameters));

	ULONG ulFetched = 0;
	for (IMethodParameterSptr spMethodParameter;
		S_OK == spMethodParameters->Next(1, &spMethodParameter, &ulFetched)
		&& spMethodParameter != nullptr;
		++usParamIndex, spMethodParameter.Detach())
	{
		IInstructionSptr spLoadArg;
		IfFailRet(
			spInstructionFactory->CreateLoadArgInstruction(
				usParamIndex,
				&spLoadArg));
		IfFailRet(spInstructionGraph->InsertAfter(
			spCurrentInstruction,
			spLoadArg));

		spCurrentInstruction = spLoadArg;

		ITypeSptr spArgType;
		IfFailRet(spMethodParameter->GetType(&spArgType));

		bool bIsValueType = false;
		mdToken tkArgVal = mdTokenNil;
		IfFailRet(GetBoxToken(spMethodInfo, spArgType, bIsValueType, tkArgVal));
		if (bIsValueType)
		{
			IInstructionSptr spBoxArgInstruction;
			IfFailRet(
				spInstructionFactory->CreateTokenOperandInstruction(
				Cee_Box,
				tkArgVal,
				&spBoxArgInstruction));
			IfFailRet(spInstructionGraph->InsertAfter(
				spCurrentInstruction,
				spBoxArgInstruction));
			spCurrentInstruction = spBoxArgInstruction;
		}

	}

	return S_OK;
}


HRESULT CLoadArgumentsHelper::GetBoxToken(
	_In_ const IMethodInfoSptr& spMethodInfo,
	_In_ const ITypeSptr& spType,
	_Out_ bool& bIsValueType,
	_Out_ mdToken& tkValueTypeToken
	)
{
	tkValueTypeToken = mdTokenNil;
	bIsValueType = false;

	CorElementType cetRetValType = ELEMENT_TYPE_END;
	IfFailRet(spType->GetCorElementType(&cetRetValType));
	bIsValueType = IsValueType(cetRetValType);

	if (bIsValueType)
	{
        TStandardTypesList::const_iterator element = m_typeMap.find(cetRetValType);
		if (element != m_typeMap.end())
		{
			auto stdType = element->second;

            IModuleInfoSptr spModuleInfo;
			IfFailRet(spMethodInfo->GetModuleInfo(&spModuleInfo));

            CReflectionHelperSptr spReflectionHelper;
            IfFailRet(CreateAndBuildUp(spReflectionHelper));
            IfFailRet(spReflectionHelper->DefineTypeToken(spModuleInfo, stdType, tkValueTypeToken));
		}
		else
		{
			ITokenTypeSptr spValTokenType;
			IfFailRet(spType->QueryInterface(&spValTokenType));

			IfFailRet(spValTokenType->GetToken(&tkValueTypeToken));
		}
	}

	return S_OK;
}


HRESULT CLoadArgumentsHelper::BoxRetValueIfNeeded(
	_In_ const IMethodInfoSptr& spMethodInfo,
	_Inout_ IInstructionSptr& spCurrentInstruction)
{
	IInstructionFactorySptr spInstructionFactory;
	IfFailRet(spMethodInfo->GetInstructionFactory(&spInstructionFactory));

	IInstructionGraphSptr spInstructionGraph;
	IfFailRet(spMethodInfo->GetInstructions(&spInstructionGraph));

	ITypeSptr spReturnType;
	IfFailRet(spMethodInfo->GetReturnType(&spReturnType));

	bool bIsValueType = false;
	mdToken tkRetVal = mdTokenNil;
	IfFailRet(GetBoxToken(spMethodInfo, spReturnType, bIsValueType, tkRetVal));

	if (bIsValueType)
	{
		IInstructionSptr spBoxRetValParamenterInstruction;
		IfFailRet(
			spInstructionFactory->CreateTokenOperandInstruction(
			Cee_Box,
			tkRetVal,
			&spBoxRetValParamenterInstruction));
		IfFailRet(spInstructionGraph->InsertAfter(
			spCurrentInstruction,
			spBoxRetValParamenterInstruction));
		spCurrentInstruction = spBoxRetValParamenterInstruction;
	}
	return S_OK;
}

HRESULT CLoadArgumentsHelper::UnBoxRetValueIfNeeded(
	_In_ const IMethodInfoSptr& spMethodInfo,
	_Inout_ IInstructionSptr& spCurrentInstruction)
{
	IInstructionFactorySptr spInstructionFactory;
	IfFailRet(spMethodInfo->GetInstructionFactory(&spInstructionFactory));

	IInstructionGraphSptr spInstructionGraph;
	IfFailRet(spMethodInfo->GetInstructions(&spInstructionGraph));

	ITypeSptr spReturnType;
	IfFailRet(spMethodInfo->GetReturnType(&spReturnType));

	bool bIsValueType = false;
	mdToken tkRetVal = mdTokenNil;
	IfFailRet(GetBoxToken(spMethodInfo, spReturnType, bIsValueType, tkRetVal));

	if (bIsValueType)
	{
		IInstructionSptr spBoxRetValParamenterInstruction;
		IfFailRet(
			spInstructionFactory->CreateTokenOperandInstruction(
			Cee_Unbox_Any,
			tkRetVal,
			&spBoxRetValParamenterInstruction));
		IfFailRet(spInstructionGraph->InsertAfter(
			spCurrentInstruction,
			spBoxRetValParamenterInstruction));
		spCurrentInstruction = spBoxRetValParamenterInstruction;
	}
	else
	{
		//TODO: This will only work for classes, not for byte arrays. Consider to update this logic
		ITokenTypeSptr spTokenType;
		HRESULT hrTokenType = spReturnType->QueryInterface(&spTokenType);

		if (SUCCEEDED(hrTokenType))
		{
			mdToken tkToken;

			IfFailRet(spTokenType->GetToken(&tkToken));

			IInstructionSptr spBoxRetValCastInstruction;
			IfFailRet(
				spInstructionFactory->CreateTokenOperandInstruction(
				Cee_Castclass,
				tkToken,
				&spBoxRetValCastInstruction));
			IfFailRet(spInstructionGraph->InsertAfter(
				spCurrentInstruction,
				spBoxRetValCastInstruction));
			spCurrentInstruction = spBoxRetValCastInstruction;
		}
	}
	return S_OK;
}
