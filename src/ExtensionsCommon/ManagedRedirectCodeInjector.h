// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ICodeInjector.h"
#include "ProfilerHostServices.h"
#include "WellKnownMethodInfo.h"
#include "ReflectionHelper.h"
#include "MethodRecord.h"

class CManagedRedirectCodeInjector final
    : public CProfilerHostServices
    , public ICodeInjector
{
private:
    const std::unordered_map<std::wstring, std::wstring> m_namesMapping;
    CReflectionHelperSptr m_spReflectionHelper;

public:
    CManagedRedirectCodeInjector();

    HRESULT BuildUpMembers();

    void AddRedirect();

    HRESULT Inject(_In_ const IMethodInfoSptr& sptrMethodInfo, _In_ const CMethodRecordSptr& spMethodRecord) override final;

    HRESULT EmitModule(_In_ const IModuleInfoSptr& sptrModuleInfo) override final;
};
