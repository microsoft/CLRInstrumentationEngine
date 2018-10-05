// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "ProfilerHostServices.h"
#include "WellKnownMethodInfo.h"

class CReflectionHelper final
    : public CProfilerHostServices
{
public:
    CReflectionHelper() noexcept {}
    virtual ~CReflectionHelper() {}

	_Success_(return == 0)
    HRESULT GetAssemblyRefByName(
        const IModuleInfoSptr& spModuleInfo,
        const std::wstring& assemblyName,
        _Out_ mdToken& tkAssemblyRef) const;

    HRESULT IsTypeBelongsToModule(
        _In_ const IModuleInfoSptr& spModuleInfo,
        _In_ const std::shared_ptr<Agent::Reflection::CWellKnownTypeInfo> spWellKnownTypeInfo,
        _Out_ bool& bBelong);

    HRESULT GetMethodToken(
        _In_ const IModuleInfoSptr& spModuleInfo,
        _In_ const std::shared_ptr<Agent::Reflection::CWellKnownMethodInfo> spWellKnownMethodInfo,
        _Out_ mdToken& tkMethodToken);

    HRESULT DefineMethodToken(
        _In_ const IModuleInfoSptr& spModuleInfo,
        _In_ const std::shared_ptr<Agent::Reflection::CWellKnownMethodInfo> spWellKnownMethodInfo,
        _Out_ mdToken& tkMethodToken);

    HRESULT GetTypeToken(
        _In_ const IModuleInfoSptr& spModuleInfo,
        _In_ const std::shared_ptr<Agent::Reflection::CWellKnownTypeInfo> spWellKnownTypeInfo,
        _Out_ mdToken& tkTypeToken);

    HRESULT DefineTypeToken(
        _In_ const IModuleInfoSptr& spModuleInfo,
        _In_ const std::shared_ptr<Agent::Reflection::CWellKnownTypeInfo> spWellKnownTypeInfo,
        _Out_ mdToken& tkTypeToken);

private:
    HRESULT GetOrDefineMethodToken(
        _In_ const IModuleInfoSptr& spModuleInfo,
        _In_ const std::shared_ptr<Agent::Reflection::CWellKnownMethodInfo> spWellKnownMethodInfo,
        _Out_ mdToken& tkMethodToken,
        _In_ bool bDefine);

    HRESULT GetOrDefineTypeToken(
        _In_ const IModuleInfoSptr& spModuleInfo,
        _In_ const std::shared_ptr<Agent::Reflection::CWellKnownTypeInfo> spWellKnownTypeInfo,
        _Out_ mdToken& tkTypeToken,
        _In_ bool bDefine);

};

typedef std::shared_ptr<CReflectionHelper> CReflectionHelperSptr;

