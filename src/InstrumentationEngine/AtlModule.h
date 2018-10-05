// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "ProfilerManager.h"
#include <string>

namespace MicrosoftInstrumentationEngine
{
    class CCustomAtlModule : public ATL::CAtlDllModuleT<CCustomAtlModule>
    {
    public:
        CCustomAtlModule();
        virtual ~CCustomAtlModule();

    private:
        /// Find and load the correct satellite resource dll for the profiler
        HRESULT LoadResourceModule();

        void FreeResourceModule();

    public:
        DECLARE_NO_REGISTRY() // don't try to register the module.
    };
}
