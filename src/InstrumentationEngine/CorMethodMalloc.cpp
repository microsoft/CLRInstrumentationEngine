// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "CorMethodMalloc.h"

MicrosoftInstrumentationEngine::CCorMethodMalloc::CCorMethodMalloc() : m_cbBufferLen(0)
{
    DEFINE_REFCOUNT_NAME(CCorMethodMalloc);
}


PVOID MicrosoftInstrumentationEngine::CCorMethodMalloc::Alloc(_In_ ULONG cb)
{
    m_pBuffer.Free();

    m_pBuffer.Allocate(cb);
    if (m_pBuffer == nullptr)
    {
        CLogging::LogError(_T("CCorMethodMalloc::Alloc failed"));
        return NULL;
    }
    m_cbBufferLen = cb;

    return (PVOID)(m_pBuffer.m_p);
}

// Called by CCorProfilerInfoWrapper to obtain the last allocation and validate the buffer
// NOTE: Relies on the serialization of jit callbacks for thread safety.
HRESULT MicrosoftInstrumentationEngine::CCorMethodMalloc::GetCurrentBufferAndLen(_Out_ BYTE** ppBuffer, _Out_ ULONG* pcbBufferLen)
{
    *ppBuffer = m_pBuffer.m_p;
    *pcbBufferLen = m_cbBufferLen;

    return S_OK;
}