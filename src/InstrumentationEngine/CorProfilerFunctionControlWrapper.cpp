// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "CorProfilerFunctionControlWrapper.h"

MicrosoftInstrumentationEngine::CCorProfilerFunctionInfoWrapper::CCorProfilerFunctionInfoWrapper(
    _In_ CProfilerManager* pProfilerManager,
    _In_ CMethodInfo* pMethodInfo
    ) : m_pProfilerManager(pProfilerManager), m_pMethodInfo(pMethodInfo)
{
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerFunctionInfoWrapper::SetCodegenFlags(
    _In_ DWORD flags)
{
    HRESULT hr = S_OK;
    DWORD originalFlags = 0;

    IfFailRet(m_pMethodInfo->GetRejitCodeGenFlags(&originalFlags));
    IfFailRet(m_pMethodInfo->SetRejitCodeGenFlags(originalFlags | flags));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerFunctionInfoWrapper::SetILFunctionBody(
    _In_ ULONG cbNewILMethodHeader,
    _In_reads_(cbNewILMethodHeader) LPCBYTE pbNewILMethodHeader)
{
    HRESULT hr = S_OK;

    IfFailRet(m_pMethodInfo->SetFinalRenderedFunctionBody(pbNewILMethodHeader, cbNewILMethodHeader, FALSE));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerFunctionInfoWrapper::SetILInstrumentedCodeMap(
    _In_ ULONG cILMapEntries,
    _In_reads_(cILMapEntries) COR_IL_MAP rgILMapEntries[  ])
{
    HRESULT hr = S_OK;

    IfFailRet(m_pMethodInfo->MergeILInstrumentedCodeMap(cILMapEntries, rgILMapEntries));

    return S_OK;
}