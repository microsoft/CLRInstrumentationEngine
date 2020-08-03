// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ExceptionSection.h"
#include "EnumeratorImpl.h"

MicrosoftInstrumentationEngine::CExceptionSection::CExceptionSection(_In_ CMethodInfo* pMethodInfo) : m_pMethodInfo(pMethodInfo)
{
    DEFINE_REFCOUNT_NAME(CExceptionSection);
    InitializeCriticalSection(&m_cs);
}

MicrosoftInstrumentationEngine::CExceptionSection::~CExceptionSection()
{
    DeleteCriticalSection(&m_cs);
    m_pMethodInfo = NULL;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionSection::Initialize(
    _In_ const IMAGE_COR_ILMETHOD* pMethodHeader,
    _In_ ULONG cbMethodSize,
    _In_ CInstructionGraph* pInstructionGraph
    )
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CExceptionSection::Initialize"));
    IfNullRetPointer(pMethodHeader);

    if(((COR_ILMETHOD_TINY*)pMethodHeader)->IsTiny() || IsFlagClear(pMethodHeader->Fat.Flags, CorILMethod_MoreSects))
    {
        CLogging::LogMessage(_T("CExceptionSection::Initialize - Method has no exception handlers"));
        return S_OK;;
    }

	const COR_ILMETHOD* pMethod = (COR_ILMETHOD*)pMethodHeader;
    const BYTE* pSectsBegin = (BYTE*)(pMethod->Fat.GetSect());

	COR_ILMETHOD_DECODER method(&(pMethod->Fat));

    const COR_ILMETHOD_SECT_EH* pCurEHSect = method.EH;

    while (pCurEHSect)
	{
        DWORD nCount = pCurEHSect->EHCount();

        if (pCurEHSect->IsFat())
		{
            const IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* pFatEHClause = pCurEHSect->Fat.Clauses;
            for(DWORD nIndex = 0; nIndex < nCount; ++nIndex)
			{
                CComPtr<CExceptionClause> pClause;
                pClause.Attach(new CExceptionClause(m_pMethodInfo));

                IfFailRet(pClause->InitializeFromFat(&pFatEHClause[nIndex], pInstructionGraph));

                m_exceptionClauses.push_back((IExceptionClause*)pClause);
            }
        }
        else
		{
            const IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_SMALL* pSmallEHClause = pCurEHSect->Small.Clauses;
            for  (DWORD nIndex = 0; nIndex < nCount; ++nIndex)
			{
                CComPtr<CExceptionClause> pClause;
                pClause.Attach(new CExceptionClause(m_pMethodInfo));

                IfFailRet(pClause->InitializeFromSmall(&pSmallEHClause[nIndex], pInstructionGraph));

                m_exceptionClauses.push_back((IExceptionClause*)pClause);
            }
        }

        // Find the next EH section
        do
		{
            pCurEHSect = (COR_ILMETHOD_SECT_EH*)pCurEHSect->Next();
        } while  (pCurEHSect && (pCurEHSect->Kind() & CorILMethod_Sect_KindMask) != CorILMethod_Sect_EHTable);

    }

    CLogging::LogMessage(_T("End CExceptionSection::Initialize"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionSection::CreateExceptionHeader(_Out_ BYTE** ppExceptionHeader, _Out_ DWORD* pcbExceptionHeader)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CExceptionSection::CreateExceptionHeader"));
    IfNullRetPointer(ppExceptionHeader);
    IfNullRetPointer(pcbExceptionHeader);
    *ppExceptionHeader = NULL;
    *pcbExceptionHeader = 0;

    if (m_exceptionClauses.empty())
    {
        return S_FALSE;
    }

    ULONG dataSize = ((ULONG)m_exceptionClauses.size()) * sizeof(IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT);

    DWORD dwBufferSize = sizeof(COR_ILMETHOD_SECT_EH_FAT) + dataSize;
    CAutoVectorPtr<BYTE> pBuffer;
    pBuffer.Allocate(dwBufferSize);
    if (pBuffer == NULL)
    {
        return E_OUTOFMEMORY;
    }

    COR_ILMETHOD_SECT_EH_FAT* pEHSect = (COR_ILMETHOD_SECT_EH_FAT*)(pBuffer.m_p);

    pEHSect->Kind = CorILMethod_Sect_EHTable | CorILMethod_Sect_FatFormat;
    pEHSect->DataSize = dataSize;

    int i=0;
    for(vector<CComPtr<IExceptionClause>>::iterator iter = m_exceptionClauses.begin(); iter != m_exceptionClauses.end(); iter++)
    {
        CComPtr<IExceptionClause> pClause = *iter;
        ((CExceptionClause*)pClause.p)->RenderExceptionClause(&pEHSect->Clauses[i]);
        i++;
    }

    *ppExceptionHeader = pBuffer.Detach();
    *pcbExceptionHeader = dwBufferSize;

    CLogging::LogMessage(_T("End CExceptionSection::CreateExceptionHeader"));
    return hr;
}


HRESULT MicrosoftInstrumentationEngine::CExceptionSection::GetMethodInfo(_Out_ IMethodInfo** ppMethodInfo)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CExceptionSection::GetMethodInfo"));
    IfNullRetPointer(ppMethodInfo);

    *ppMethodInfo = m_pMethodInfo;
    (*ppMethodInfo)->AddRef();

    CLogging::LogMessage(_T("End CExceptionSection::GetMethodInfo"));
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionSection::GetExceptionClauses(_Out_ IEnumExceptionClauses** ppEnumExceptionClauses)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CExceptionSection::GetExceptionClauses"));

    IfNullRetPointer(ppEnumExceptionClauses);
    *ppEnumExceptionClauses = NULL;

    CCriticalSectionHolder lock(&m_cs);

    CComPtr<CEnumerator<IEnumExceptionClauses, IExceptionClause>> pEnumerator;
    pEnumerator.Attach(new CEnumerator<IEnumExceptionClauses, IExceptionClause>);
    if (pEnumerator == NULL)
    {
        return E_OUTOFMEMORY;
    }

    pEnumerator->Initialize(m_exceptionClauses);

    *ppEnumExceptionClauses = pEnumerator.Detach();

    CLogging::LogMessage(_T("End CExceptionSection::GetExceptionClauses"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionSection::AddExceptionClause(_In_ IExceptionClause* pExceptionClause)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CExceptionSection::AddExceptionClause"));

    IfNullRetPointer(pExceptionClause);

    CCriticalSectionHolder lock(&m_cs);

    // TODO: Any validation needed?

    m_exceptionClauses.push_back(pExceptionClause);

    CLogging::LogMessage(_T("End CExceptionSection::AddExceptionClause"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionSection::RemoveExceptionClause(_In_ IExceptionClause* pExceptionClause)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CExceptionSection::RemoveExceptionClause"));
    IfNullRetPointer(pExceptionClause);

    CCriticalSectionHolder lock(&m_cs);

    for (vector<CComPtr<IExceptionClause>>::iterator iter = m_exceptionClauses.begin(); iter != m_exceptionClauses.end(); iter++)
    {
        if (*iter == pExceptionClause)
        {
            m_exceptionClauses.erase(iter);
            break;
        }
    }

    CLogging::LogMessage(_T("End CExceptionSection::RemoveExceptionClause"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionSection::RemoveAllExceptionClauses()
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CExceptionSection::RemoveAllExceptionClauses"));
    CCriticalSectionHolder lock(&m_cs);

    m_exceptionClauses.clear();

    CLogging::LogMessage(_T("End CExceptionSection::RemoveAllExceptionClauses"));

    return hr;
}

// Search the exception clauses for pointers to pInstructionOld and update pointers
HRESULT MicrosoftInstrumentationEngine::CExceptionSection::UpdateInstruction(_In_ CInstruction* pInstructionOld, _In_ CInstruction* pInstructionNew)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CExceptionSection::UpdateInstruction"));
    IfNullRetPointer(pInstructionOld);

    CCriticalSectionHolder lock(&m_cs);

    for (vector<CComPtr<IExceptionClause>>::iterator iter = m_exceptionClauses.begin(); iter != m_exceptionClauses.end(); iter++)
    {
        CExceptionClause* pClause = (CExceptionClause*)((*iter).p);
        pClause->UpdateInstruction(pInstructionOld, pInstructionNew);
    }

    CLogging::LogMessage(_T("End CExceptionSection::UpdateInstruction"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionSection::AddNewExceptionClause(
    _In_ DWORD flags,
    _In_ IInstruction* pTryFirstInstruction,
    _In_ IInstruction* pTryLastInstruction,
    _In_ IInstruction* pHandlerFirstInstruction,
    _In_ IInstruction* pHandlerLastInstruction,
    _In_ IInstruction* pFilterLastInstruction,
    _In_ mdToken handlerTypeToken,
    _Out_ IExceptionClause** ppExceptionClause
    )
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CExceptionSection::CreateExceptionClause"));
    IfNullRetPointer(ppExceptionClause);
    *ppExceptionClause = NULL;

    CCriticalSectionHolder lock(&m_cs);

    CComPtr<CExceptionClause> pExceptionClause;
    pExceptionClause.Attach(new CExceptionClause(m_pMethodInfo));

    IfFailRet(pExceptionClause->SetFlags(flags));
    IfFailRet(pExceptionClause->SetTryFirstInstruction(pTryFirstInstruction));
    IfFailRet(pExceptionClause->SetTryLastInstruction(pTryLastInstruction));
    IfFailRet(pExceptionClause->SetHandlerFirstInstruction(pHandlerFirstInstruction));
    IfFailRet(pExceptionClause->SetHandlerLastInstruction(pHandlerLastInstruction));
    IfFailRet(pExceptionClause->SetFilterFirstInstruction(pFilterLastInstruction));
    IfFailRet(pExceptionClause->SetExceptionHandlerType(handlerTypeToken));

    vector<CComPtr<IExceptionClause>>::iterator iter;
    IfFailRet(FindExceptionClauseInsertionPoint(pExceptionClause, iter));
    m_exceptionClauses.insert(iter, (IExceptionClause*)pExceptionClause);

    *ppExceptionClause = pExceptionClause;

    CLogging::LogMessage(_T("End CExceptionSection::CreateExceptionClause"));

    return hr;
}


HRESULT MicrosoftInstrumentationEngine::CExceptionSection::FindExceptionClauseInsertionPoint(
    _In_ CExceptionClause* pNewExceptionClause,
    _Out_ vector<CComPtr<IExceptionClause>>::iterator& iterRetVal
    )
{
    HRESULT hr = S_OK;

    iterRetVal = m_exceptionClauses.begin();

    // The CLR requires that if an exception range contains another exception range,
    // the smaller range must come first in the order of added exception clauses.
    // Reverse linear search the existing clauses to see if any of them are contained within
    // the new range. If none are found, insert at the beginning of the list.
    // INVARIANT: all previous clauses were inserted in the correct order. This means
    // the first exception clause whose try range is contained in the new block when REVERSE iterating
    // is the correct range to insert after.
    if (m_exceptionClauses.begin() != m_exceptionClauses.end())
    {
        for (vector<CComPtr<IExceptionClause>>::reverse_iterator iter = m_exceptionClauses.rbegin(); iter != m_exceptionClauses.rend(); iter++)
        {
            CComPtr<IExceptionClause> pExistingExceptionClause = *iter;

            bool bIsContained = false;
            IfFailRet(IsExistingTryContainedWithinNewTry((CExceptionClause*)(pExistingExceptionClause.p), pNewExceptionClause, &bIsContained));
            if (bIsContained)
            {
                // found one that is contained within this one. Insert after it.
                iterRetVal = iter.base();
                return S_OK;
            }
        }
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CExceptionSection::IsExistingTryContainedWithinNewTry(
    _In_ CExceptionClause* pExistingExceptionClause,
    _In_ CExceptionClause* pNewExceptionClause,
    _Out_ bool* pbIsContained
    )
{
    HRESULT hr = S_OK;

    *pbIsContained = false;

    // Get the first and last instructions in the try ranges for both clauses
    CComPtr<IInstruction> pNewTryFirstInstruction;
    CComPtr<IInstruction> pNewTryLastInstruction;
    IfFailRet(pNewExceptionClause->GetTryFirstInstruction(&pNewTryFirstInstruction));
    IfFailRet(pNewExceptionClause->GetTryLastInstruction(&pNewTryLastInstruction));

    CComPtr<IInstruction> pExistingTryFirstInstruction;
    CComPtr<IInstruction> pExistingTryLastInstruction;
    IfFailRet(pExistingExceptionClause->GetTryFirstInstruction(&pExistingTryFirstInstruction));
    IfFailRet(pExistingExceptionClause->GetTryLastInstruction(&pExistingTryLastInstruction));

    // Gather the range data.
    DWORD dwNewTryFirstOffset = 0;
    DWORD dwNewTryLastOffset = 0;
    DWORD dwNewTryLastLength = 0;
    IfFailRet(pNewTryFirstInstruction->GetOffset(&dwNewTryFirstOffset));
    IfFailRet(pNewTryLastInstruction->GetOffset(&dwNewTryLastOffset));
    IfFailRet(pNewTryLastInstruction->GetInstructionLength(&dwNewTryLastLength));

    DWORD dwNewFinalOffset = dwNewTryLastOffset + dwNewTryLastLength;

    DWORD dwExistingTryFirstOffset = 0;
    DWORD dwExistingTryLastOffset = 0;
    DWORD dwExistingTryLastLength = 0;
    IfFailRet(pExistingTryFirstInstruction->GetOffset(&dwExistingTryFirstOffset));
    IfFailRet(pExistingTryLastInstruction->GetOffset(&dwExistingTryLastOffset));
    IfFailRet(pExistingTryLastInstruction->GetInstructionLength(&dwExistingTryLastLength));

    DWORD dwExistingFinalOffset = dwExistingTryLastOffset + dwExistingTryLastLength;

    // Do the comparison.
    if (dwNewTryFirstOffset <= dwExistingTryFirstOffset)
    {
        if (dwExistingFinalOffset <= dwNewFinalOffset)
        {
            *pbIsContained = true;
            return S_OK;
        }
    }

    return S_OK;
}