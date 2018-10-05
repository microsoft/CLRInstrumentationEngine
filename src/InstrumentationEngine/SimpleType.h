// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "Type.h"
#include <cor.h>

namespace MicrosoftInstrumentationEngine
{
    class CSimpleType : public CType
    {
        public:
            friend class CTypeCreator;
        protected:
            CSimpleType(_In_ CorElementType corType);
    };
}