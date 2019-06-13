// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "MethodParameter.h"

MicrosoftInstrumentationEngine::CMethodParameter::CMethodParameter(_In_ IType* parameterType) : m_pType(parameterType)
{
    DEFINE_REFCOUNT_NAME(CMethodParameter);
}

HRESULT MicrosoftInstrumentationEngine::CMethodParameter::GetType(_Out_ IType** ppType)
{
    IfNullRetPointer(ppType);
    return m_pType->QueryInterface(ppType);
}