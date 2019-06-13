// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"

#include "ProfilerManagerHostBase.h"

#include "InstrumentationEnginePointerDefs.h"
#include "AgentValidation.h"

_Check_return_ HRESULT STDMETHODCALLTYPE CProfilerManagerHostBase::Initialize(
    _In_ IProfilerManager* pHost)
{
    IfTrueRet(nullptr == pHost, E_INVALIDARG);
    IProfilerManagerSptr sptrIProfilerManager(pHost);
    IfTrueRet(nullptr == sptrIProfilerManager, E_INVALIDARG);

    IProfilerManagerLoggingSptr spLogger;
    IfFailRet(sptrIProfilerManager->GetLoggingInstance(&spLogger));
    this->SetLogger(spLogger);

    IfFailRet(this->InternalInitialize(sptrIProfilerManager));

    return S_OK;
}

_Check_return_ HRESULT CProfilerManagerHostBase::InternalInitialize(
    _In_ const IProfilerManagerSptr&)
{
    return S_OK;
}