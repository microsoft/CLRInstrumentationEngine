// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "Instruction.h"
#include "InstructionGraph.h"

MicrosoftInstrumentationEngine::CInstructionGraph::CInstructionGraph() : m_pMethodInfo(NULL), m_bHasBaselineBeenSet(false), m_bAreInstructionsStale(true)
{
    DEFINE_REFCOUNT_NAME(CInstructionGraph);
    InitializeCriticalSection(&m_cs);
}

/*
Destruction of graph should remove pointers between instructions so it will not cause
cascading destruction of all instructions which can lead to stack overflow for bigger methods.

Example (first and last nodes contains still 2 references as graph has pointers to first and last nodes):

Initial state:
|=node 1=| ---> |=node 2=| ---> |=node 3=| ---> |=node 4=|
|        |      |        |      |        |      |        |
| ref: 2 |      | ref: 2 |      | ref: 2 |      | ref: 2 |
|========| <--- |========| <--- |========| <--- |========|

After node 1 disconnect:
|=node 1=|      |=node 2=| ---> |=node 3=| ---> |=node 4=|
|        |      |        |      |        |      |        |
| ref: 2 |      | ref: 1 |      | ref: 2 |      | ref: 2 |
|========| <--- |========| <--- |========| <--- |========|

After node 2 disconnect:
|=node 1=|      |=node 2=|      |=node 3=| ---> |=node 4=|
|        |      |        |      |        |      |        |
| ref: 1 |      | ref: 1 |      | ref: 1 |      | ref: 2 |
|========|      |========| <--- |========| <--- |========|

After node 3 disconnect:
|=node 1=|      |=node 2=|      |=node 3=|      |=node 4=|
|        |      |        |      |        |      |        |
| ref: 1 |      | ref: 0 |      | ref: 1 |      | ref: 1 |
|========|      |========|      |========| <--- |========|

After node 4 disconnect:
|=node 1=|      |=node 2=|      |=node 3=|      |=node 4=|
|        |      |        |      |        |      |        |
| ref: 1 |      | ref: 0 |      | ref: 0 |      | ref: 1 |
|========|      |========|      |========|      |========|
*/
MicrosoftInstrumentationEngine::CInstructionGraph::~CInstructionGraph()
{
    {
        CCriticalSectionHolder lock(&m_cs);

        CInstruction* pInstr = m_pFirstInstruction;
        while (pInstr != nullptr)
        {
            CInstruction* pNextInstruction = pInstr->NextInstructionInternal();
            pInstr->Disconnect();
            pInstr = pNextInstruction;
        }
    }

    DeleteCriticalSection(&m_cs);
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::Initialize(_In_ CMethodInfo* pMethodInfo)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CInstructionGraph::Initialize"));

    m_pMethodInfo = pMethodInfo;

    CLogging::LogMessage(_T("End CInstructionGraph::Initialize"));

    return hr;
}

// Before baseline:
//
//                  [Instruction    ]  [Instruction ]  [Instruction]
//                  |               |  |            |  |           |
//      pFirst----->|     next ------->|    next------>|           |
//                  |               |  |            |  |           |
//      pOriginal-->|  nextOriginal--->|  nextOrig---->|           |
//                  |_______________|  |____________|  |___________|

// Afterbaseline:

//                   [Instruction    ]  [Instruction ]  [Instruction]
//                   |               |  |            |  |           |
//       pFirst----->|     next ------->|    next------>|           |
//                   |               |  |            |  |           |
//       pOriginal-->|  nextOriginal--->|  nextOrig---->|           |
//                   |_______________|  |____________|  |___________|
//
//                              [ Baseline_IL_map]
//
//                   [Instruction    ]  [Instruction ]  [Instruction]
//                   |               |  |            |  |           |
//                   |IsRemoved      |  |IsRemoved   |  |IsRemoved  |
//                   |               |  |            |  |           |
// pUninstrumented-->|  nextOriginal--->|  nextOrig---->|           |
//                   |_______________|  |____________|  |___________|


HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::CreateBaseline(
    _In_ LPCBYTE pCodeBase,
    _In_ LPCBYTE pEndOfCode,
    _In_ DWORD originalToBaselineCorIlMapSize,
    _In_ COR_IL_MAP originalToBaselineCorIlMap[],
    _In_ DWORD baselineSequencePointSize,
    _In_ DWORD baselineSequencePointList[]
    )
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pCodeBase);
    IfNullRetPointer(pEndOfCode);

    if (pCodeBase >= pEndOfCode)
    {
        return E_INVALIDARG;
    }

    if (originalToBaselineCorIlMapSize > 0)
    {
        IfNullRetPointer(originalToBaselineCorIlMap);
    }

    if (baselineSequencePointSize > 0)
    {
        IfNullRetPointer(baselineSequencePointList);
    }

    if (m_pUninstrumentedFirstInstruction != nullptr)
    {
        return E_UNEXPECTED;
    }

    if (!m_pMethodInfo->IsCreateBaselineEnabled())
    {
        CLogging::LogError(_T("CInstructionGraph::CreateBaseline - Baseline can only be set during BeforeInstrumentMethod."));
        return E_FAIL;
    }

    if (m_bHasBaselineBeenSet)
    {
        CLogging::LogError(_T("CInstructionGraph::CreateBaseline - Baseline has already been set."));
        return E_FAIL;
    }
    m_bHasBaselineBeenSet = true;

    IfFailRet(RemoveAll());

    m_pUninstrumentedFirstInstruction.Attach(m_pOrigFirstInstruction.Detach());
    m_pUninstrumentedLastInstruction.Attach(m_pOrigLastInstruction.Detach());

    IfFailRet(DecodeInstructions(pCodeBase, pEndOfCode));

    CInstruction* pCurrInstruction = m_pOrigFirstInstruction;

    while (pCurrInstruction != NULL)
    {
        IfFailRet(pCurrInstruction->SetInstructionGeneration(InstructionGeneration::Generation_Baseline));
        pCurrInstruction = pCurrInstruction->NextInstructionInternal();
    }

    if (originalToBaselineCorIlMapSize > 0)
    {
        m_originalToBaselineIlMap.resize(originalToBaselineCorIlMapSize);
        for (DWORD i = 0; i < originalToBaselineCorIlMapSize; i++)
        {
            m_originalToBaselineIlMap[i] = originalToBaselineCorIlMap[i];
        }
    }

    if (baselineSequencePointSize > 0)
    {
        m_baselineIlMap.resize(baselineSequencePointSize);
        for (DWORD j = 0; j < baselineSequencePointSize; j++)
        {
            m_baselineIlMap[j].oldOffset = baselineSequencePointList[j];
            m_baselineIlMap[j].newOffset = baselineSequencePointList[j];
            m_baselineIlMap[j].fAccurate = true;
        }

        IfFailRet(m_pMethodInfo->MergeILInstrumentedCodeMap(baselineSequencePointSize, m_baselineIlMap.data()));
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::GetInstructionAtUninstrumentedOffset(_In_ DWORD dwOffset, _Out_ IInstruction** ppInstruction)
{
    if (m_pUninstrumentedFirstInstruction == nullptr)
    {
        return GetInstructionAtOriginalOffset(dwOffset, ppInstruction);
    }

    DWORD offsetToSearch = dwOffset;

    {
        CCriticalSectionHolder lock(&m_cs);
        for (const COR_IL_MAP& map : m_originalToBaselineIlMap)
        {
            if (map.oldOffset == dwOffset)
            {
                //Found exact offset
                offsetToSearch = map.newOffset;
                return GetInstructionAtOffset(offsetToSearch, ppInstruction);
            }

            //We will only map at a sequence point boundary. If the baseline code has small differences
            //from the original, interpolating the offset won't work.
        }
    }
    return E_FAIL;
}

// Builds a list of instruction from an IL image
HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::DecodeInstructions(_In_ LPCBYTE pCodeBase, _In_ LPCBYTE pEndOfCode)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::Decode"));

    CCriticalSectionHolder lock(&m_cs);

    const BYTE * pCode = (BYTE*)pCodeBase;


    if (pEndOfCode > pCode)
    {
        IfFailRet(CInstruction::InstructionFromBytes(pCode, pEndOfCode, &m_pFirstInstruction));
    }

    unordered_map<ULONG, CInstruction*> offsetMap;

    m_pOrigFirstInstruction = m_pFirstInstruction;

    // First pass, get the opcodes
    CComPtr<CInstruction> pNewInstr = m_pFirstInstruction;

    CInstruction* pPrevious = nullptr;
    while (pNewInstr)
    {
        IfFailRet(pNewInstr->SetGraph(this));
        IfFailRet(pNewInstr->SetPreviousInstruction(pPrevious, true));

        if (pPrevious != nullptr)
        {
            pPrevious->SetNextInstruction(pNewInstr, true);
        }

        ULONG offset = (ULONG)(pCode - pCodeBase);

        pNewInstr->SetOffset(offset);
        pNewInstr->SetOriginalOffset(offset);

        // Add the instruction to the offset map. We don't need
        // to ref count because the backward link above will
        // keep the pointer alive.
        offsetMap[offset] = pNewInstr.p;
        pPrevious = pNewInstr.p;

        // Jump to the next instruction
        DWORD instructionSize = 0;
        IfFailRet(pNewInstr->GetInstructionSize(&instructionSize));
        pCode += instructionSize;

        if (pCode >= pEndOfCode)
        {
            break;
        }

        // Creating the new instruction will addref
        pNewInstr.Release();
        IfFailRet(CInstruction::InstructionFromBytes(pCode, pEndOfCode, &pNewInstr));
    }

    // These do not need to be CComPtrs because they memory that they represent
    // will be reference counted inside the offset map, as well as the backward links
    // in the graph.
    CInstruction* pCurrent = pPrevious;
    CInstruction* pLast = pCurrent;
    CInstruction* pNext = nullptr;

    // Intellitrace made the graph circular. I really don't like this design though as the circular references
    // will need to be broken. I'm making GetPreviousInstruction return null for the first instruction.
    // pCurrent->SetPreviousInstruction(pLast);

    // Second pass, go backwards through the instructions and finalize branch instructions
    while (pCurrent)
    {
        // Finalize branch pointers
        bool isBranch = pCurrent->GetIsBranchInternal();
        bool isSwitch = pCurrent->GetIsSwitchInternal();

        if (isBranch)
        {
            // We should maybe do a QI here to be sure.
            CBranchInstruction* pBranch = static_cast<CBranchInstruction*>(pCurrent);

            DWORD dwTargetOffset = 0;
            pBranch->GetTargetOffset(&dwTargetOffset);

            DWORD dwBranchOrigOffset = 0;
            pCurrent->GetOriginalOffset(&dwBranchOrigOffset);

            ULONG offset = dwTargetOffset + dwBranchOrigOffset;

            unordered_map<ULONG, CInstruction*>::iterator iter = offsetMap.find(offset);
            if (iter == offsetMap.end())
            {
                CLogging::LogError(_T("DecodeInstructions - Invalid program. Branch target does not exist"));
                return E_FAIL;
            }

            pBranch->SetBranchTarget(iter->second);
        }
        else if (isSwitch)
        {
            CSwitchInstruction* pSwitch = static_cast<CSwitchInstruction*>(pCurrent);
            CInstruction* pNextInner = pCurrent->NextInstructionInternal();
            IfFalseRet(pNextInner != nullptr, E_FAIL);

            ULONG nextOffset = 0;
            IfFailRet(pNextInner->GetOriginalOffset(&nextOffset));

            DWORD branchCount = 0;
            IfFailRet(pSwitch->GetBranchCount(&branchCount));

            for (ULONG i = 0; i < branchCount; i++)
            {
                ULONG offset = 0;
                IfFailRet(pSwitch->GetBranchOffset(i, &offset));

                unordered_map<ULONG, CInstruction*>::iterator iter = offsetMap.find(offset + nextOffset);
                if (iter == offsetMap.end())
                {
                    CLogging::LogError(_T("DecodeInstructions - Invalid program. Switch target does not exist"));
                    return E_FAIL;
                }

                // Eventualy set the branch to the 'nop' before the ret so that epilog can be added between the nop and the ret
                pSwitch->SetBranchTarget(i, iter->second);
            }
        }

        pNext = pCurrent;
        pCurrent = pNext->PreviousInstructionInternal();
    }

    // Intellitrace made the graph circular. I really don't like this design though as the circular references
    // will need to be broken. I'm making GetPreviousInstruction return null for the first instruction.
    // m_pFirstInstruction->SetPreviousInstruction(pLast);

    m_pLastInstruction = pLast;
    m_pOrigLastInstruction = pLast;

    return S_OK;
}

// Generate an IL image from a list of instructions. At the same time produce a map from the old instructions to the new ones.
HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::EncodeIL(
    _Out_ CAutoVectorPtr<BYTE>* ppILBuffer,
    _Out_ DWORD* pdwILStreamLen,
    _Out_ CAutoVectorPtr<COR_IL_MAP>* ppCorILMap,
    _Out_ DWORD* pdwCorILMapmLen
    )
{
    HRESULT hr = S_OK;
    ULONG cCorILMap = 0;
    ULONG cbBuffer = 0;
    *pdwILStreamLen = 0;
    *pdwCorILMapmLen = 0;

    CCriticalSectionHolder lock(&m_cs);

    // Count the number of il map entries
    CInstruction* pInstruction = m_pFirstInstruction;
    while (pInstruction != NULL)
    {
        DWORD dwInstructionSize = 0;
        IfFailRet(pInstruction->GetInstructionSize(&dwInstructionSize));

        BOOL bIsNewInstruction = FALSE;
        pInstruction->GetIsNew(&bIsNewInstruction);

        if (!bIsNewInstruction)
        {
            cCorILMap++;
        }

        cbBuffer += dwInstructionSize;
        pInstruction = pInstruction->NextInstructionInternal();
    }

    IfFailRet(CalculateInstructionOffsets());

    // Allocate the il buffer
    CAutoVectorPtr<BYTE> pILArray;
    if (cbBuffer > 0)
    {
        pILArray.Attach(new BYTE[cbBuffer]);
    }

    // Allocate the il map.
    CAutoVectorPtr<COR_IL_MAP> pCorILMap;
    if(cCorILMap > 0)
    {
        pCorILMap.Attach(new COR_IL_MAP[cCorILMap]);
    }

    DWORD iCorILMap = 0;

    ULONG pos = 0;
    CInstruction* pCurrent = m_pFirstInstruction;

    // The first new instruction between the last old instruction and the current
    // old instrution
    CInstruction* pPrevNewInstruction = NULL;
    while (pCurrent != NULL)
    {
        // Encode this instruction
        IfFailRet(pCurrent->EmitIL(pILArray, cbBuffer));

        BOOL bIsNewInstruction = FALSE;
        pCurrent->GetIsNew(&bIsNewInstruction);

        if (!bIsNewInstruction)
        {
            DWORD currOldOffset;
            IfFailRet(pCurrent->GetOriginalOffset(&currOldOffset));

            DWORD currOffset;
            IfFailRet(pCurrent->GetOffset(&currOffset));

            if (pPrevNewInstruction != NULL)
            {
                // If there exists a new instruction between the last old instruction
                // and the current old instruction, use the offset of that instruction.
                // The CLR goes forward on the new offsets, which means without this, the
                // old instructions would map to the last instruction instead of the current instruction.
                // See the documentation of COR_IL_MAP for an example. This is because we typically
                // want the inserted instructions to be associated with the _next_ instruction in the
                // graph, not the previous one. Otherwise, if the debugger stops in an instrumented
                // portion of the code, it will associate the break with the
                // previous line instead of the next line.
                DWORD currPrevNewInstructionOffset = 0;
                IfFailRet(pPrevNewInstruction->GetOffset(&currPrevNewInstructionOffset));

                pCorILMap[iCorILMap].fAccurate = true;
                pCorILMap[iCorILMap].oldOffset = currOldOffset;
                pCorILMap[iCorILMap].newOffset = currPrevNewInstructionOffset;
                iCorILMap++;
            }
            else
            {
                pCorILMap[iCorILMap].fAccurate = true;
                pCorILMap[iCorILMap].oldOffset = currOldOffset;
                pCorILMap[iCorILMap].newOffset = currOffset;
                iCorILMap++;
            }

            pPrevNewInstruction = NULL;
        }
        else
        {
            if (pPrevNewInstruction == NULL)
            {
                pPrevNewInstruction = pCurrent;
            }
        }

        // Move to the next instruction
        pCurrent = pCurrent->NextInstructionInternal();
    }

    // The map that was just generated points all new instructions to the previous source line, which is incorrect.
    // Make a reverse pass back through the il map entries setting the new offset to the last instruction that
    // was

    *ppILBuffer = pILArray;
    *pdwILStreamLen = cbBuffer;
    *ppCorILMap = pCorILMap;
    *pdwCorILMapmLen = cCorILMap;

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::GetInstructionAtOffset(_In_ DWORD offset, _Out_ CInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CCriticalSectionHolder lock(&m_cs);

    CInstruction* pCurrent = m_pFirstInstruction;
    while (pCurrent != NULL)
    {
        DWORD currOffset = 0;
        IfFailRet(pCurrent->GetOffset(&currOffset));

        if (currOffset == offset)
        {
            *ppInstruction = pCurrent;
            pCurrent->AddRef();
            return S_OK;
        }

        pCurrent = pCurrent->NextInstructionInternal();
    }

    return E_FAIL;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::GetInstructionAtEndOffset(_In_ DWORD offset, _Out_ CInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CCriticalSectionHolder lock(&m_cs);

    CInstruction* pCurrent = m_pFirstInstruction;
    while (pCurrent != NULL)
    {
        DWORD currOffset = 0;
        IfFailRet(pCurrent->GetOffset(&currOffset));

        DWORD currSize = 0;
        IfFailRet(pCurrent->GetInstructionSize(&currSize));

        if ((currOffset + currSize) == offset)
        {
            *ppInstruction = pCurrent;
            pCurrent->AddRef();
            return S_OK;
        }

        pCurrent = pCurrent->NextInstructionInternal();
    }

    return E_FAIL;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::CalculateInstructionOffsets()
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::CalculateInstructionOffsets"));
    CCriticalSectionHolder lock(&m_cs);

    ULONG pos = 0;
    CInstruction* pCurrent = m_pFirstInstruction;
    CInstruction* pPrev = NULL;
    while (pCurrent != NULL)
    {
        DWORD dwPrevOffset = 0;
        DWORD dwPrevLength = 0;
        if (pPrev != NULL)
        {
            IfFailRet(pPrev->GetOffset(&dwPrevOffset));
            IfFailRet(pPrev->GetInstructionSize(&dwPrevLength));
        }

        IfFailRet(pCurrent->SetOffset(dwPrevOffset + dwPrevLength));

        pPrev = pCurrent;
        pCurrent = pCurrent->NextInstructionInternal();
    }

    CLogging::LogMessage(_T("End CInstructionGraph::CalculateInstructionOffsets"));
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::ExpandBranches()
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::ExpandBranches"));
    CCriticalSectionHolder lock(&m_cs);

    ULONG pos = 0;
    CInstruction* pCurrent = m_pFirstInstruction;
    CInstruction* pPrev = NULL;
    while (pCurrent != NULL)
    {
        bool bIsBranch = pCurrent->GetIsBranchInternal();

        if (bIsBranch)
        {
            ((CBranchInstruction*)pCurrent)->ExpandBranch();
        }

        pPrev = pCurrent;
        pCurrent = pCurrent->NextInstructionInternal();
    }

    MarkInstructionsStale();
    CLogging::LogMessage(_T("End CInstructionGraph::ExpandBranches"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::GetMethodInfo(_Out_ IMethodInfo** ppMethodInfo)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CInstructionGraph::Initialize"));
    IfNullRetPointer(ppMethodInfo);

    if (!m_pMethodInfo)
    {
        return E_FAIL;
    }

    *ppMethodInfo = m_pMethodInfo;
    (*ppMethodInfo)->AddRef();

    CLogging::LogMessage(_T("End CInstructionGraph::Initialize"));

    return hr;
}

// Get the current first and last instructions reflecting changes that instrumentation methods have made
HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::GetFirstInstruction(_Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::GetFirstInstruction"));
    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(ppInstruction);

    *ppInstruction = (IInstruction*)(m_pFirstInstruction.p);
    if (*ppInstruction)
    {
        (*ppInstruction)->AddRef();
    }

    CLogging::LogMessage(_T("End CInstructionGraph::GetFirstInstruction"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::GetLastInstruction(_Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::GetLastInstruction"));
    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(ppInstruction);

    *ppInstruction = (IInstruction*)(m_pLastInstruction.p);
    if (*ppInstruction)
    {
        (*ppInstruction)->AddRef();
    }

    CLogging::LogMessage(_T("End CInstructionGraph::GetLastInstruction"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::GetOriginalFirstInstruction(_Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::GetOriginalFirstInstruction"));
    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(ppInstruction);

    *ppInstruction = (IInstruction*)(m_pOrigFirstInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionGraph::GetOriginalFirstInstruction"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::GetOriginalLastInstruction(_Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::GetOriginalLastInstruction"));
    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(ppInstruction);

    *ppInstruction = (IInstruction*)(m_pOrigLastInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionGraph::GetOriginalLastInstruction"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::GetUninstrumentedFirstInstruction(_Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::GetUninstrumentedFirstInstruction"));
    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(ppInstruction);
    *ppInstruction = nullptr;
    if (m_pUninstrumentedFirstInstruction == nullptr)
    {
        //TODO Should we return original instruction instead?
        return S_FALSE;
    }

    hr = m_pUninstrumentedFirstInstruction.QueryInterface(ppInstruction);

    CLogging::LogMessage(_T("End CInstructionGraph::GetUninstrumentedFirstInstruction"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::GetUninstrumentedLastInstruction(_Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::GetUninstrumentedFirstInstruction"));
    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(ppInstruction);
    *ppInstruction = nullptr;
    if (m_pUninstrumentedLastInstruction == nullptr)
    {
        //TODO Should we return original instruction instead?
        return S_FALSE;
    }
    hr = m_pUninstrumentedLastInstruction.QueryInterface(ppInstruction);

    CLogging::LogMessage(_T("End CInstructionGraph::GetUninstrumentedFirstInstruction"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::GetInstructionAtOffset(_In_  DWORD offset, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::GetInstructionAtOffset"));
    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(ppInstruction);

    CInstruction* pCurrInstruction = m_pFirstInstruction;

    while (pCurrInstruction != NULL)
    {
        DWORD currOffset = 0;
        IfFailRet(pCurrInstruction->GetOffset(&currOffset));

        if (currOffset == offset)
        {
            *ppInstruction = pCurrInstruction;
            (*ppInstruction)->AddRef();
            return S_OK;
        }

        pCurrInstruction = pCurrInstruction->NextInstructionInternal();
    }

    CLogging::LogMessage(_T("End CInstructionGraph::GetInstructionAtOffset"));

    return E_FAIL;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::GetInstructionAtOriginalOffset(_In_  DWORD offset, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::GetInstructionAtOriginalOffset"));
    CCriticalSectionHolder lock(&m_cs);
    IfNullRetPointer(ppInstruction);

    CInstruction* pCurrInstruction = m_pOrigFirstInstruction;

    while (pCurrInstruction != NULL)
    {
        DWORD currOffset = 0;
        IfFailRet(pCurrInstruction->GetOriginalOffset(&currOffset));

        BOOL bIsNew = FALSE;
        IfFailRet(pCurrInstruction->GetIsNew(&bIsNew));

        if (!bIsNew)
        {
            if (currOffset == offset)
            {
                *ppInstruction = pCurrInstruction;
                (*ppInstruction)->AddRef();
                return S_OK;
            }
            pCurrInstruction = pCurrInstruction->OriginalNextInstructionInternal();
        }
        else
        {
            pCurrInstruction = pCurrInstruction->NextInstructionInternal();
        }
    }

    CLogging::LogMessage(_T("End CInstructionGraph::GetInstructionAtOriginalOffset"));

    return E_FAIL;
}

// Insert an instruction before another instruction. jmp offsets that point to the original instruction
// are not updated to reflect this change
HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::InsertBefore(_In_ IInstruction* pInstructionOrig, _In_ IInstruction* pInstructionNew)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::InsertBefore"));
    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(pInstructionOrig);
    IfNullRetPointer(pInstructionNew);

    CInstruction* pInstrOrig = (CInstruction*)pInstructionOrig;
    CInstruction* pInstrNew = (CInstruction*)pInstructionNew;

    IfFalseRet(pInstrOrig->GetGraph() == this, E_UNEXPECTED);
    IfFailRet(pInstrNew->SetGraph(this));

    CComPtr<CInstruction> pPreviousInstruction;
    pInstructionOrig->GetPreviousInstruction((IInstruction**)(&pPreviousInstruction));

    IfFailRet(pInstrOrig->SetPreviousInstruction(pInstrNew, false));
    IfFailRet(pInstrNew->SetNextInstruction(pInstrOrig, false));

    if (pPreviousInstruction != NULL)
    {
        if (pPreviousInstruction != pInstructionNew)
        {
            IfFailRet(pPreviousInstruction->SetNextInstruction(pInstrNew, false));
            IfFailRet(pInstrNew->SetPreviousInstruction(pPreviousInstruction, false));
        }
    }
    else
    {
        // original instruction must have been the the first instruction in the graph.
        m_pFirstInstruction = pInstrNew;
    }

    
    // NOTE: Unlike intellitrace's engine, the offsets of instructions are updated after each change.
    // This is necessary since this is an API to be consumed by multiple instermentation methods.
    MarkInstructionsStale();

    CLogging::LogMessage(_T("End CInstructionGraph::InsertBefore"));

    return hr;
}

// Insert an instruction after another instruction. jmp offsets that point to the next instruction after
// the other instruction are not updated to reflect this change
HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::InsertAfter(_In_ IInstruction* pInstructionOrig, _In_ IInstruction* pInstructionNew)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::InsertAfter"));
    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(pInstructionNew);

    CInstruction* pInstrOrig = (CInstruction*)pInstructionOrig;
    CInstruction* pInstrNew = (CInstruction*)pInstructionNew;
    IfFailRet(pInstrNew->SetGraph(this));

    IfFailRet(pInstrNew->SetPreviousInstruction(pInstrOrig, false));

    CInstruction* pNextInstruction = NULL;
    if (pInstructionOrig != NULL)
    {
        IfFalseRet(pInstrOrig->GetGraph() == this, E_UNEXPECTED);
        pNextInstruction = pInstrOrig->NextInstructionInternal();
        IfFailRet(pInstrOrig->SetNextInstruction(pInstrNew, false));
    }
    else
    {
        m_pFirstInstruction = pInstrNew;
    }

    if (pNextInstruction != NULL)
    {
        if (pNextInstruction != pInstrNew)
        {
            IfFailRet(pNextInstruction->SetPreviousInstruction(pInstrNew, false));
            IfFailRet(pInstrNew->SetNextInstruction(pNextInstruction, false));
        }
    }
    else
    {
        // original instruction must have been the the last instruction in the graph.
        m_pLastInstruction = pInstrNew;
    }

    // NOTE: Unlike intellitrace's engine, the offsets of instructions are updated after each change.
    // This is necessary since this is an API to be consumed by multiple instermentation methods.
    MarkInstructionsStale();

    CLogging::LogMessage(_T("End CInstructionGraph::InsertAfter"));

    return hr;
}

// Insert an instruction before another instruction AND update jmp targets and exception ranges that used
// to point to the old instruction to point to the new instruction.
HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::InsertBeforeAndRetargetOffsets(_In_ IInstruction* pInstructionOrig, _In_ IInstruction* pInstructionNew)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CInstructionGraph::InsertBeforeAndRetargetOffsets"));
    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(pInstructionOrig);
    IfNullRetPointer(pInstructionNew);

    CInstruction* pInstrOrig = (CInstruction*)pInstructionOrig;
    CInstruction* pInstrNew = (CInstruction*)pInstructionNew;
    IfFalseRet(pInstrOrig->GetGraph() == this, E_UNEXPECTED);
    IfFailRet(pInstrNew->SetGraph(this));

    CComPtr<CInstruction> pPreviousInstruction;
    pInstructionOrig->GetPreviousInstruction((IInstruction**)(&pPreviousInstruction));

    IfFailRet(pInstrOrig->SetPreviousInstruction(pInstrNew, false));
    IfFailRet(pInstrNew->SetNextInstruction(pInstrOrig, false));

    if (pPreviousInstruction != NULL)
    {
        if (pPreviousInstruction != pInstructionNew)
        {
            IfFailRet(pPreviousInstruction->SetNextInstruction(pInstrNew, false));
            IfFailRet(pInstrNew->SetPreviousInstruction(pPreviousInstruction, false));
        }
    }
    else
    {
        // original instruction must have been the the first instruction in the graph.
        m_pFirstInstruction = pInstrNew;
    }

    // NOTE: Unlike intellitrace's engine, the offsets of instructions are updated after each change.
    // This is necessary since this is an API to be consumed by multiple instrumentation methods.
    MarkInstructionsStale();

    // Search the graph for any branch whose branch target is the old instruction. Set the branch target
    // to the new instruction.
    CInstruction* pCurr = m_pFirstInstruction;
    while (pCurr != nullptr)
    {
        bool isBranch = pCurr->GetIsBranchInternal();

        if (isBranch)
        {
            CComPtr<CBranchInstruction> pBranch = (CBranchInstruction*)pCurr;

            CInstruction* pBranchTarget = pBranch->GetBranchTargetInternal();

            // If the branch target is the original instruction AND the branch is not the new instruction,
            // update it. Note the second condition is important because for things like leave instructions,
            // often the next instruction is the branch target and resetting this would create an infinite loop.
            if (pBranchTarget == pInstructionOrig && pCurr != pInstructionNew)
            {
                IfFailRet(pBranch->SetBranchTarget(pInstructionNew));
            }
        }
        else
        {
            bool isSwitch = pCurr->GetIsSwitchInternal();

            if (isSwitch)
            {
                CComPtr<ISwitchInstruction> pSwitch;
                IfFailRet(pCurr->QueryInterface(__uuidof(ISwitchInstruction), (LPVOID*)&pSwitch));
                IfFailRet(pSwitch->ReplaceBranchTarget(pInstructionOrig, pInstructionNew));
            }

        }

        pCurr = pCurr->NextInstructionInternal();
    }

    // Search the exception clauses for anything that points to the old instruction
    // Set that target to the new instruction
    if (m_pMethodInfo != NULL)
    {
        CComPtr<CExceptionSection> pExceptionSection;
        IfFailRet(m_pMethodInfo->GetExceptionSection((IExceptionSection**)&pExceptionSection));
        if (pExceptionSection != NULL)
        {
            pExceptionSection->UpdateInstruction(pInstrOrig, pInstrNew);
        }
    }

    CLogging::LogMessage(_T("End CInstructionGraph::InsertBeforeAndRetargetOffsets"));

    return hr;
}


// Replace an instruction with another instruction. The old instruction continues to live in the original graph but is marked replaced
HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::Replace(_In_ IInstruction* pInstructionOrig, _In_ IInstruction *pInstructionNew)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::Replace"));
    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(pInstructionOrig);
    IfNullRetPointer(pInstructionNew);

    CInstruction* pInstrOrig = (CInstruction*)pInstructionOrig;
    CInstruction* pInstrNew = (CInstruction*)pInstructionNew;
    IfFalseRet(pInstrOrig->GetGraph() == this, E_UNEXPECTED);
    IfFailRet(pInstrNew->SetGraph(this));

    CInstruction* pPreviousInstruction = pInstrOrig->PreviousInstructionInternal();
    CInstruction* pNextInstruction = pInstrOrig->NextInstructionInternal();

    pInstrOrig->SetIsRemoved();

    if (pPreviousInstruction != NULL)
    {
        IfFailRet(pPreviousInstruction->SetNextInstruction(pInstrNew, false));
        IfFailRet(pInstrNew->SetPreviousInstruction(pPreviousInstruction, false));
    }
    else
    {
        m_pFirstInstruction = pInstrNew;
    }

    if (pNextInstruction != NULL)
    {
        IfFailRet(pNextInstruction->SetPreviousInstruction(pInstrNew, false));
        IfFailRet(pInstrNew->SetNextInstruction(pNextInstruction, false));
    }
    else
    {
        m_pLastInstruction = pInstrNew;
    }

    // NOTE: Unlike intellitrace's engine, the offsets of instructions are updated after each change.
    // This is necessary since this is an API to be consumed by multiple instermentation methods.
    MarkInstructionsStale();

    // Search the exception clauses for anything that points to the old instruction
    // Set that target to the new instruction
    if (m_pMethodInfo != NULL)
    {
        CComPtr<CExceptionSection> pExceptionSection;
        IfFailRet(m_pMethodInfo->GetExceptionSection((IExceptionSection**)&pExceptionSection));
        if (pExceptionSection != NULL)
        {
            pExceptionSection->UpdateInstruction(pInstrOrig, pInstrNew);
        }
    }

    CLogging::LogMessage(_T("End CInstructionGraph::Replace"));

    return hr;
}

// Remove an instruction with another instruction. The old instruction continues to live in the original graph but is marked deleted
HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::Remove(_In_ IInstruction *pInstruction)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CInstructionGraph::Remove"));
    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(pInstruction);

    CComPtr<CInstruction> pInstr = (CInstruction*)pInstruction;
    IfFalseRet(pInstr->GetGraph() == this, E_UNEXPECTED);

    CComPtr<CInstruction> pPreviousInstruction;
    CComPtr<CInstruction> pNextInstruction;

    pInstruction->GetPreviousInstruction((IInstruction**)(&pPreviousInstruction));
    pInstruction->GetNextInstruction((IInstruction**)(&pNextInstruction));

    pInstr->SetIsRemoved();

    if (pPreviousInstruction != NULL)
    {
        IfFailRet(pPreviousInstruction->SetNextInstruction(pNextInstruction, false));
    }
    else
    {
        m_pFirstInstruction = pNextInstruction;
    }

    if (pNextInstruction != NULL)
    {
        IfFailRet(pNextInstruction->SetPreviousInstruction(pPreviousInstruction, false));
    }
    else
    {
        m_pLastInstruction = pPreviousInstruction;
    }

    // NOTE: Unlike intellitrace's engine, the offsets of instructions are updated after each change.
    // This is necessary since this is an API to be consumed by multiple instermentation methods.
    MarkInstructionsStale();

    // Search the exception clauses for anything that points to the old instruction
    // Set that target to the next instruction. IF there isn't a next instruction, then this
    // is the last instruction in the method. Set the exception clause to the previous
    if (m_pMethodInfo != NULL)
    {
        CComPtr<CExceptionSection> pExceptionSection;
        IfFailRet(m_pMethodInfo->GetExceptionSection((IExceptionSection**)&pExceptionSection));
        if (pExceptionSection != NULL)
        {
            if (pNextInstruction == NULL && pPreviousInstruction == NULL)
            {
                CLogging::LogError(_T("CInstructionGraph::Remove - Invalid program"));
                return E_FAIL;
            }

            if (pNextInstruction)
            {
                pExceptionSection->UpdateInstruction(pInstr, pNextInstruction);
            }
            else
            {
                pExceptionSection->UpdateInstruction(pInstr, pPreviousInstruction);
            }
        }
    }

    CLogging::LogMessage(_T("End CInstructionGraph::Remove"));

    return hr;
}

// Remove all instructions from the current graph. The original instructions are still accessible from the original first and original last
// methods. These are all marked deleted  and their original next fields are still set.
HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::RemoveAll()
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionGraph::RemoveAll"));
    CCriticalSectionHolder lock(&m_cs);

    CInstruction* pInstr = (CInstruction*)m_pFirstInstruction;

    while (pInstr != NULL)
    {
        pInstr->SetIsRemoved();

        CInstruction* pPreviousInstruction = pInstr->PreviousInstructionInternal();
        CInstruction* pNextInstruction = pInstr->NextInstructionInternal();

        if (pPreviousInstruction != NULL)
        {
            IfFailRet(pPreviousInstruction->SetNextInstruction(NULL, false));
        }

        if (pNextInstruction != NULL)
        {
            IfFailRet(pNextInstruction->SetPreviousInstruction(NULL, false));
        }

        IfFailRet(pInstr->SetNextInstruction(NULL, false));
        IfFailRet(pInstr->SetPreviousInstruction(NULL, false));

        pInstr = pNextInstruction;
    }

    m_pFirstInstruction = NULL;
    m_pLastInstruction = NULL;

    // Remove the existing exception clauses.
    if (m_pMethodInfo != NULL)
    {
        CComPtr<CExceptionSection> pExceptionSection;
        IfFailRet(m_pMethodInfo->GetExceptionSection((IExceptionSection**)&pExceptionSection));
        IfFailRet(pExceptionSection->RemoveAllExceptionClauses());
    }

    CLogging::LogMessage(_T("End CInstructionGraph::Remove"));

    return hr;
}


HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::CalculateMaxStack(_Out_ DWORD* pMaxStack)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pMaxStack);
    *pMaxStack = 0;

    DWORD maxStackDepth = 0;
    DWORD stackDepth = 0;

    // Heuristic to calculate maxstack. This may guess sligthly high as control flow
    // is not taken into account. It makes a linear pass over the instructions looking at
    // the stack impact of each instruction and maintaining a theoretical maximum.
    // The actual maximum may be slightly smaller.
    CInstruction* pInstr = (CInstruction*)m_pFirstInstruction;
    while (pInstr != NULL)
    {
        CInstruction* pNextInstruction = pInstr->NextInstructionInternal();

        int stackImpact = 0;
        IfFailRet(pInstr->GetStackImpact(m_pMethodInfo, stackDepth, &stackImpact));

        stackDepth += stackImpact;

        // If this instruction is the first instruction in a catch block,
        // the stack will be emptied first, and the exception object will be pushed
        // on the stack by the runtime
        bool isFirstInstructionInCatch = false;
        IfFailRet(IsFirstInstructionInCatch(pInstr, &isFirstInstructionInCatch));
        if (isFirstInstructionInCatch)
        {
            stackDepth = 1;
        }

        if (stackDepth > maxStackDepth)
        {
            maxStackDepth = stackDepth;
        }

        pInstr = pNextInstruction;
    }

    *pMaxStack = maxStackDepth;

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::IsFirstInstructionInCatch(_In_ IInstruction* pInstr, _Out_ bool* pIsFirstInstructionInCatch)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pIsFirstInstructionInCatch);
    *pIsFirstInstructionInCatch = false;

    CComPtr<IExceptionSection> pExceptionSection;
    IfFailRet(m_pMethodInfo->GetExceptionSection(&pExceptionSection));

    CComPtr<IEnumExceptionClauses> pEnumExceptionClauses;
    IfFailRet(pExceptionSection->GetExceptionClauses(&pEnumExceptionClauses));

    DWORD cActual = 0;
    CComPtr<IExceptionClause> pExceptionClause;
    pEnumExceptionClauses->Next(1, &pExceptionClause, &cActual);

    while (pExceptionClause != nullptr)
    {
        DWORD exceptionFlags;
        IfFailRet(pExceptionClause->GetFlags(&exceptionFlags));

        if (exceptionFlags == COR_ILEXCEPTION_CLAUSE_NONE)
        {
            CComPtr<IInstruction> pCurrHandlerInstr;
            IfFailRet(pExceptionClause->GetHandlerFirstInstruction(&pCurrHandlerInstr));

            if (pCurrHandlerInstr.IsEqualObject(pInstr))
            {
                *pIsFirstInstructionInCatch = true;
                break;
            }
        }

        pExceptionClause.Release();
        pEnumExceptionClauses->Next(1, &pExceptionClause, &cActual);
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::RefreshInstructions()
{
    CCriticalSectionHolder lock(&m_cs);
    if (m_bAreInstructionsStale)
    {
        m_bAreInstructionsStale = false;
        return CalculateInstructionOffsets();
    }

    return S_OK;
}

// true if CreateBaseline has previously been called.
HRESULT MicrosoftInstrumentationEngine::CInstructionGraph::HasBaselineBeenSet(_Out_ BOOL* pHasBaselineBeenSet)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pHasBaselineBeenSet);

    *pHasBaselineBeenSet = m_bHasBaselineBeenSet;

    return hr;
}
