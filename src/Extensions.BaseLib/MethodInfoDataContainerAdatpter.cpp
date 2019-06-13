// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "ExtensionsCommon\MethodInfoDataContainerAdatpter.h"

namespace Agent
{
    namespace Interop
    {
        //NOTE: IT IS IMPORTANT THAT EVERY INSTRUMENTATION METHOD DEFINES IT'S OWN GUID.
        //DO NOT MOVE THIS FILE TO COMMON FOLDER

        // GUID for identifying data while reading/writing from/to IMethodInfoSptr.
        // We store a MethodRecordSptr in IMethodInfoSptr between InternalShouldInstrumentMethod and InternalInstrumentMethod.
        // {3306763F-91D5-4585-B801-BB729EB53316}
        static const GUID GUID_MethodInfoDataItem = { /* 41be92e9-6e2a-4d3a-bdbc-e2c82670b79e */
            0x41be92e9,
            0x6e2a,
            0x4d3a,
            { 0xbd, 0xbc, 0xe2, 0xc8, 0x26, 0x70, 0xb7, 0x9e }
        };
    }
}