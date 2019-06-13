// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "InstrumentationEngineDefs.h"
#include "ProfilerHostServices.h"

class CProfilerManagerHostBase
    : public IProfilerManagerHost
    , public virtual CProfilerHostServices
{
public:
    _Check_return_ virtual HRESULT STDMETHODCALLTYPE Initialize(
        _In_ IProfilerManager* pProfilerManager) final;

    virtual ~CProfilerManagerHostBase() {}

protected:
    _Check_return_ virtual HRESULT InternalInitialize(
        _In_ const IProfilerManagerSptr& sptrHost);
}; // ProfilerManagerHostBase