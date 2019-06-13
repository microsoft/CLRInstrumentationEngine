// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "DataItemContainer.h"
#include "DataContainerAdapter.h"
#include "NativeInstanceMethodInstrumentationInfo.h"

namespace Agent
{
    namespace Interop
    {
        extern const GUID GUID_MethodInfoDataItem;

        typedef CDataContainerAdapterImplT <
            Instrumentation::Native::CNativeInstanceMethodInstrumentationInfoSptr,
            IMethodInfoSptr,
            &GUID_MethodInfoDataItem > CMethodInfoDataContainerAdapter;

        typedef std::shared_ptr<CMethodInfoDataContainerAdapter> CMethodInfoDataContainerAdapterSptr;
    }
}