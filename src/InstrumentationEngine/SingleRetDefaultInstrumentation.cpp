// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "SingleRetDefaultInstrumentation.h"

MicrosoftInstrumentationEngine::CSingleRetDefaultInstrumentation::CSingleRetDefaultInstrumentation()
{
}

MicrosoftInstrumentationEngine::CSingleRetDefaultInstrumentation::~CSingleRetDefaultInstrumentation()
{
}

HRESULT MicrosoftInstrumentationEngine::CSingleRetDefaultInstrumentation::Initialize(
    _In_ IInstructionGraph* pInstructionGraph
    )
{
    m_pInstructionGraph = pInstructionGraph;
    return S_OK;
}


// This is the implementation of the single ret default instrumentation which is applied to any method
// that is instrumented by an instrumentation method or by the raw profiler hook if the DefaultInstrumentationType_SingleRet
// flag is set on the profiler manager.
//
// This replaces all of the rets in the function with branches that jmp to a single ret at the end of the function.
// The advantage to this is it makes collecting the return value significantly easier for instrumentation.
HRESULT MicrosoftInstrumentationEngine::CSingleRetDefaultInstrumentation::ApplySingleRetDefaultInstrumentation()
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CSingleRetDefaultInstrumentation::ApplySingleRetDefaultInstrumentation"));

    CComPtr<IMethodInfo> pMethodInfo;
    IfFailRet(m_pInstructionGraph->GetMethodInfo(&pMethodInfo));

    // Find all of the ret instructions in the instruction graph
    vector<CComPtr<IInstruction>> retInstructions;
    IfFailRet(GetReplaceableRets(retInstructions));

    if (retInstructions.size() == 0)
    {
        // the original method does not have any replacable ret instructions.
        // This can happen when there's endless loop, or the function ends in throw
        CLogging::LogMessage(_T("CSingleRetDefaultInstrumentation::ApplySingleRetDefaultInstrumentation - function contains no replacable ret instructions"));
        return S_OK;
    }
    else if (retInstructions.size() == 1)
    {
        CLogging::LogMessage(_T("CSingleRetDefaultInstrumentation::ApplySingleRetDefaultInstrumentation - function only contains a single replacable ret instruction. No transform will be applied"));
        return S_OK;
    }

    // Add the new ret instruction to the end of the method.
    CComPtr<IInstructionFactory> pInstructionFactory;
    IfFailRet(pMethodInfo->GetInstructionFactory(&pInstructionFactory));

    CComPtr<IInstruction> pNewRetInstruction;
    IfFailRet(pInstructionFactory->CreateInstruction(Cee_Ret, &pNewRetInstruction));

    CComPtr<IInstruction> pLastInstruction;
    IfFailRet(m_pInstructionGraph->GetLastInstruction(&pLastInstruction));
    IfFailRet(m_pInstructionGraph->InsertAfter(pLastInstruction, pNewRetInstruction));

    // If the method has a return value, the value must be popped off the stack and into a local slot if possible.
    // the new ret instruction will load it back out of the local slot before the final ret
    CComPtr<IType> pRetType;
    IfFailRet(pMethodInfo->GetReturnType(&pRetType));
    CorElementType retTypeElementType;
    IfFailRet(pRetType->GetCorElementType(&retTypeElementType));

    bool bStoreRetVal = retTypeElementType != ELEMENT_TYPE_VOID && retTypeElementType != ELEMENT_TYPE_BYREF;

    // Store the instruction where the old rets should now point
    CComPtr<IInstruction> pBranchTarget = pNewRetInstruction;
    DWORD dwRetValTempSlotIndex = 0;
    if (bStoreRetVal)
    {
        CComPtr<ILocalVariableCollection> pLocalVariables;
        pMethodInfo->GetLocalVariables(&pLocalVariables);

        IfFailRet(pLocalVariables->AddLocal(pRetType, &dwRetValTempSlotIndex));

        // Insert a loadlocal before the new ret instruction
        CComPtr<IInstruction> pNewLoadLocalInstruction;
        IfFailRet(pInstructionFactory->CreateUShortOperandInstruction(Cee_Ldloc, (USHORT)dwRetValTempSlotIndex, &pNewLoadLocalInstruction));

        IfFailRet(m_pInstructionGraph->InsertBeforeAndRetargetOffsets(pNewRetInstruction, pNewLoadLocalInstruction));

        // Set the jmp target to the load local
        pBranchTarget = pNewLoadLocalInstruction;
    }

    // for each of the old rets, replace them with a jmp to the new ret.
    // If needed, store the return value in the new local.
    for (CComPtr<IInstruction> pCurrRetInstruction : retInstructions)
    {
        CComPtr<IInstruction> pNewBranchInstruction;
        IfFailRet(pInstructionFactory->CreateBranchInstruction(Cee_Br, pBranchTarget, &pNewBranchInstruction));

        IfFailRet(m_pInstructionGraph->Replace(pCurrRetInstruction, pNewBranchInstruction));

        if (bStoreRetVal)
        {
            // Insert a store loc into the new temporary return value slot
            CComPtr<IInstruction> pStoreLocalInstruction;
            IfFailRet(pInstructionFactory->CreateUShortOperandInstruction(Cee_Stloc, (USHORT)dwRetValTempSlotIndex, &pStoreLocalInstruction));

            IfFailRet(m_pInstructionGraph->InsertBeforeAndRetargetOffsets(pNewBranchInstruction, pStoreLocalInstruction));
        }
    }

    m_pJumpTarget = pBranchTarget;
    CLogging::LogMessage(_T("End CSingleRetDefaultInstrumentation::ApplySingleRetDefaultInstrumentation"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CSingleRetDefaultInstrumentation::GetBranchTargetInstruction(_Out_ IInstruction** ppJumpTarget)
{
    IfNullRet(ppJumpTarget);
    *ppJumpTarget = nullptr;
    if (m_pJumpTarget != nullptr)
    {
        return m_pJumpTarget.CopyTo(ppJumpTarget);
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CSingleRetDefaultInstrumentation::GetReplaceableRets(_In_ vector<CComPtr<IInstruction>>& retInstructions)
{
    HRESULT hr = S_OK;

    CComPtr<IInstruction> pInstruction;
    IfFailRet(m_pInstructionGraph->GetFirstInstruction(&pInstruction));

    while (pInstruction != NULL)
    {
        ILOrdinalOpcode opcode;
        IfFailRet(pInstruction->GetOpCode(&opcode));

        if (opcode == Cee_Ret)
        {
            retInstructions.push_back(pInstruction);
        }

        CComPtr<IInstruction> pCurr = pInstruction;
        pInstruction.Release();
        pCurr->GetNextInstruction(&pInstruction);
    }

    // Remove all non-replacable rets from the list.
    // If ret is immediately preceded by tail.{call,calli,callvirt}, or tail.call/pop, don't include it.
    // it's forbidden to have instructions between tail.call and ret
    // It is fine not to replace the ret with branch in this case as tail.call means the control won't really get to it anyway
    for (vector<CComPtr<IInstruction>>::iterator iter = retInstructions.begin(); iter != retInstructions.end(); ++iter)
    {
        CComPtr<IInstruction> pCurrRet = *iter;

        CComPtr<IInstruction> pPrevInstruction;
        pCurrRet->GetPreviousInstruction(&pPrevInstruction);

        if (pPrevInstruction != nullptr)
        {
            ILOrdinalOpcode prevOpcode;
            IfFailRet(pPrevInstruction->GetOpCode(&prevOpcode));

            if (prevOpcode == Cee_Pop)
            {
                CComPtr<IInstruction> pTemp = pPrevInstruction;
                pPrevInstruction.Release();
                pTemp->GetPreviousInstruction(&pPrevInstruction);

                if (pPrevInstruction == nullptr)
                {
                    continue;
                }
            }

            BOOL isCallInstruction = FALSE;
            IfFailRet(pPrevInstruction->GetIsCallInstruction(&isCallInstruction));
            if (isCallInstruction)
            {
                CComPtr<IInstruction> pPreviousPreviousInstruction;
                pPrevInstruction->GetPreviousInstruction(&pPreviousPreviousInstruction);

                if (pPreviousPreviousInstruction == nullptr)
                {
                    continue;
                }

                ILOrdinalOpcode prevprevOpcode;
                pPreviousPreviousInstruction->GetOpCode(&prevprevOpcode);
                if (prevprevOpcode == Cee_Tailcall)
                {
                    // This is a non-replaceable ret instruction. Remove it from collection
                    vector<CComPtr<IInstruction>>::iterator erased = retInstructions.erase(iter);

                    // backtrack one on the iterator so that the next increment continues past the last return
                    iter = erased - 1;
                }
            }
        }
    }

    return hr;
}