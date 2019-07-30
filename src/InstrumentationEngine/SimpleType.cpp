// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "SimpleType.h"

MicrosoftInstrumentationEngine::CSimpleType::CSimpleType(_In_ CorElementType corType) : CType(corType)
{
    DEFINE_REFCOUNT_NAME(CSimpleType);
}