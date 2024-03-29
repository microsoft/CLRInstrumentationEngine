// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ExceptionClause.h"
#include "InstructionGraph.h"

MicrosoftInstrumentationEngine::CExceptionClause::CExceptionClause(CMethodInfo* m_pMethodInfo) :
    m_ExceptionHandlerType(mdTokenNil),
    m_pMethodInfo(m_pMethodInfo)
{
    DEFINE_REFCOUNT_NAME(CExceptionClause);
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::InitializeFromSmall(
    _In_ const IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_SMALL* pSmallClause,
    _In_ CInstructionGraph* pInstructionGraph
    )
{
    HRESULT hr = S_OK;
#ifndef _WIN64
    m_flags = pSmallClause->Flags;
#else
    m_flags = static_cast<CorExceptionFlag>(pSmallClause->Flags);
#endif

    IfFailRet(pInstructionGraph->GetInstructionAtOffsetInternal(pSmallClause->TryOffset, &m_pTryFirstInstruction));
    IfFailRet(pInstructionGraph->GetInstructionAtEndOffset(pSmallClause->TryOffset + pSmallClause->TryLength, &m_pTryLastInstruction));

    // Is it legal to not have a handler for a filter? Not doing if fail ret here.
    pInstructionGraph->GetInstructionAtOffsetInternal(pSmallClause->HandlerOffset, &m_pHandlerFirstInstruction);
    pInstructionGraph->GetInstructionAtEndOffset(pSmallClause->HandlerOffset + pSmallClause->HandlerLength, &m_pHandlerLastInstruction);

    if (m_flags == COR_ILEXCEPTION_CLAUSE_NONE)
    {
        //catch exception block - get exception type
        m_ExceptionHandlerType = pSmallClause->ClassToken;
    }
    else if (m_flags == COR_ILEXCEPTION_CLAUSE_FILTER)
    {
        //filter block - get the filter first instruction
        IfFailRet(pInstructionGraph->GetInstructionAtOffsetInternal(pSmallClause->FilterOffset, &m_pFilterFirstInstruction));
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::InitializeFromFat(
    _In_ const IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* pFatClause,
    _In_ CInstructionGraph* pInstructionGraph
    )
{
    HRESULT hr = S_OK;

    m_flags = pFatClause->Flags;

    IfFailRet(pInstructionGraph->GetInstructionAtOffsetInternal(pFatClause->TryOffset, &m_pTryFirstInstruction));
    IfFailRet(pInstructionGraph->GetInstructionAtEndOffset(pFatClause->TryOffset + pFatClause->TryLength, &m_pTryLastInstruction));

    // Is it legal to not have a handler for a filter? Not doing if fail ret here.
    pInstructionGraph->GetInstructionAtOffsetInternal(pFatClause->HandlerOffset, &m_pHandlerFirstInstruction);
    pInstructionGraph->GetInstructionAtEndOffset(pFatClause->HandlerOffset + pFatClause->HandlerLength, &m_pHandlerLastInstruction);

    if (m_flags == COR_ILEXCEPTION_CLAUSE_NONE)
    {
        //catch exception block - get exception type
        m_ExceptionHandlerType = pFatClause->ClassToken;
    }
    else if (m_flags == COR_ILEXCEPTION_CLAUSE_FILTER)
    {
        //filter block - get the filter first instruction
        IfFailRet(pInstructionGraph->GetInstructionAtOffsetInternal(pFatClause->FilterOffset, &m_pFilterFirstInstruction));
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::RenderExceptionClause(_In_ IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* pEHClause)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pEHClause);

    IfNullRet(m_pTryFirstInstruction);
    IfNullRet(m_pTryLastInstruction);
    IfNullRet(m_pHandlerFirstInstruction);
    IfNullRet(m_pHandlerLastInstruction);

    pEHClause->Flags = m_flags;

    DWORD firstTryOffset = 0;
    DWORD lastTryOffset = 0;
    DWORD lastTrySize = m_pTryLastInstruction->GetInstructionSize();;
    IfFailRet(m_pTryFirstInstruction->GetOffset(&firstTryOffset));
    IfFailRet(m_pTryLastInstruction->GetOffset(&lastTryOffset));

    DWORD firstHandlerOffset = 0;
    DWORD lastHandlerOffset = 0;
    DWORD lastHandlerSize = m_pHandlerLastInstruction->GetInstructionSize();
    IfFailRet(m_pHandlerFirstInstruction->GetOffset(&firstHandlerOffset));
    IfFailRet(m_pHandlerLastInstruction->GetOffset(&lastHandlerOffset));

    pEHClause->TryOffset = firstTryOffset;
    pEHClause->TryLength = (lastTryOffset + lastTrySize) - firstTryOffset;
    pEHClause->HandlerOffset = firstHandlerOffset;
    pEHClause->HandlerLength = (lastHandlerOffset + lastHandlerSize) - firstHandlerOffset;

    CLogging::LogMessage(_T("CExceptionClause::RenderExceptionClause, pEHClause->TryOffset: %i (%X), pEHClause->TryLength: %i (%X), pEHClause->HandlerOffset: %i (%X), pEHClause->HandlerLength: %i (%X)"),
        pEHClause->TryOffset, pEHClause->TryOffset, pEHClause->TryLength, pEHClause->TryLength, pEHClause->HandlerOffset, pEHClause->HandlerOffset, pEHClause->HandlerLength, pEHClause->HandlerLength);


    if (m_flags == COR_ILEXCEPTION_CLAUSE_NONE)
    {
        pEHClause->ClassToken = m_ExceptionHandlerType;
        CLogging::LogMessage(_T("CExceptionClause::RenderExceptionClause, pEHClause->ClassToken: %i (%X)"),
            pEHClause->ClassToken, pEHClause->ClassToken);
    }
    else if (m_flags == COR_ILEXCEPTION_CLAUSE_FILTER)
    {
        IfFailRet(m_pFilterFirstInstruction->GetOffset(&pEHClause->FilterOffset));
    }

    return hr;
}

// Search the instruction pointers for pInstructionOld and update pointers
HRESULT MicrosoftInstrumentationEngine::CExceptionClause::UpdateInstruction(_In_ CInstruction* pInstructionOld, _In_ CInstruction* pInstructionNew)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pInstructionOld);

    // This function is to update the exception clause range for InsertBefore, Remove and Replace instructions.
    // We try to include both old and new instruction in the range. For which the *FirstInstruction should always
    // point to the new instruction. And the *LastInstruction fields should keep pointing to the old instruction,
    // unless the old one is removed.

    // TODO: Make this logic to work with InsertAfter.

    if (m_pTryFirstInstruction == pInstructionOld)
    {
        m_pTryFirstInstruction = pInstructionNew;
    }

    if (m_pTryLastInstruction == pInstructionOld)
    {
        BOOL isRemoved = FALSE;
        if (SUCCEEDED(pInstructionOld->GetIsRemoved(&isRemoved)) && isRemoved)
        {
            m_pTryLastInstruction = pInstructionNew;
        }
    }

    if (m_pHandlerFirstInstruction == pInstructionOld)
    {
        m_pHandlerFirstInstruction = pInstructionNew;
    }

    if (m_pHandlerLastInstruction == pInstructionOld)
    {
        BOOL isRemoved = FALSE;
        if (SUCCEEDED(pInstructionOld->GetIsRemoved(&isRemoved)) && isRemoved)
        {
            m_pHandlerLastInstruction = pInstructionNew;
        }
    }

    if (m_pFilterFirstInstruction == pInstructionOld)
    {
        m_pFilterFirstInstruction = pInstructionNew;
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::GetFlags(_Out_ DWORD* pFlags)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pFlags);

    *pFlags = m_flags;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::GetTryFirstInstruction(_Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppInstruction);
    *ppInstruction = NULL;

    if (m_pTryFirstInstruction != NULL)
    {
        *ppInstruction = m_pTryFirstInstruction.p;
        (*ppInstruction)->AddRef();
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::GetTryLastInstruction(_Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppInstruction);
    *ppInstruction = NULL;

    if (m_pTryLastInstruction != NULL)
    {
        *ppInstruction = m_pTryLastInstruction.p;
        (*ppInstruction)->AddRef();
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::GetHandlerFirstInstruction(_Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppInstruction);
    *ppInstruction = NULL;

    if (m_pHandlerFirstInstruction != NULL)
    {
        *ppInstruction = m_pHandlerFirstInstruction.p;
        (*ppInstruction)->AddRef();
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::GetHandlerLastInstruction(_Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppInstruction);
    *ppInstruction = NULL;

    if (m_pHandlerLastInstruction != NULL)
    {
        *ppInstruction = m_pHandlerLastInstruction.p;
        (*ppInstruction)->AddRef();
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::GetFilterFirstInstruction(_Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppInstruction);
    *ppInstruction = NULL;

    if (m_pFilterFirstInstruction != NULL)
    {
        *ppInstruction = m_pFilterFirstInstruction.p;
        (*ppInstruction)->AddRef();
    }

    return hr;
}

// The token of the type in the catch block
HRESULT MicrosoftInstrumentationEngine::CExceptionClause::GetExceptionHandlerType(_Out_ mdToken* pToken)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pToken);

    *pToken = m_ExceptionHandlerType;

    return hr;
}


// flags is CorExceptionFlag
HRESULT MicrosoftInstrumentationEngine::CExceptionClause::SetFlags(_In_ DWORD flags)
{
    HRESULT hr = S_OK;

    m_flags = (CorExceptionFlag)flags;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::SetTryFirstInstruction(_In_ IInstruction* pInstruction)
{
    HRESULT hr = S_OK;

    if (pInstruction != nullptr)
    {
        IfFailRet(pInstruction->QueryInterface(__uuidof(CInstruction), (void**)&m_pTryFirstInstruction));
    }
    else
    {
        m_pTryFirstInstruction = nullptr;
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::SetTryLastInstruction(_In_ IInstruction* pInstruction)
{
    HRESULT hr = S_OK;

    if (pInstruction != nullptr)
    {
        IfFailRet(pInstruction->QueryInterface(__uuidof(CInstruction), (void**)&m_pTryLastInstruction));
    }
    else
    {
        m_pTryLastInstruction = nullptr;
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::SetHandlerFirstInstruction(_In_ IInstruction* pInstruction)
{
    HRESULT hr = S_OK;

    if (pInstruction != nullptr)
    {
        IfFailRet(pInstruction->QueryInterface(__uuidof(CInstruction), (void**)&m_pHandlerFirstInstruction));
    }
    else
    {
        m_pHandlerFirstInstruction = nullptr;
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::SetHandlerLastInstruction(_In_ IInstruction* pInstruction)
{
    HRESULT hr = S_OK;

    if (pInstruction != nullptr)
    {
        IfFailRet(pInstruction->QueryInterface(__uuidof(CInstruction), (void**)&m_pHandlerLastInstruction));
    }
    else
    {
        m_pHandlerLastInstruction = nullptr;
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionClause::SetFilterFirstInstruction(_In_ IInstruction* pInstruction)
{
    HRESULT hr = S_OK;

    if (pInstruction != nullptr)
    {
        IfFailRet(pInstruction->QueryInterface(__uuidof(CInstruction), (void**)&m_pFilterFirstInstruction));
    }
    else
    {
        m_pFilterFirstInstruction = nullptr;
    }

    return hr;
}

// The token of the type in the catch block
HRESULT MicrosoftInstrumentationEngine::CExceptionClause::SetExceptionHandlerType(_In_ mdToken token)
{
    HRESULT hr = S_OK;

    m_ExceptionHandlerType = token;

    return hr;
}