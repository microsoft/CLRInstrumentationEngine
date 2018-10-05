// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "MethodLocal.h"

MicrosoftInstrumentationEngine::CMethodLocal::CMethodLocal(_In_ IType* pType) :
    m_pType(pType)
{
    DEFINE_REFCOUNT_NAME(CMethodLocal);
}

HRESULT MicrosoftInstrumentationEngine::CMethodLocal::GetType(_Out_ IType** ppType)
{
    IfNullRetPointer(ppType);

    return m_pType->QueryInterface(ppType);
}