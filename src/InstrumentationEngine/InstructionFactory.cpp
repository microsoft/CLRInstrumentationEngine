// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "InstructionFactory.h"
#include "InstructionGraph.h"

MicrosoftInstrumentationEngine::CInstructionFactory::CInstructionFactory()
{
    DEFINE_REFCOUNT_NAME(CInstructionFactory);
}

// Create an instance of an instruction that takes no operands
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateInstruction(_In_ enum ILOrdinalOpcode opcode, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<CInstruction> pInstruction;
    pInstruction.Attach(new CInstruction(opcode, TRUE));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateInstruction"));
    return S_OK;
}

// Create an instance of an instruction that takes a byte operand
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateByteOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ BYTE operand, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateByteOperandInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<COperandInstruction> pInstruction;
    pInstruction.Attach(new COperandInstruction(opcode, TRUE, sizeof(BYTE), &operand));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateByteOperandInstruction"));

    return S_OK;
}

// Create an instance of an instruction that takes a USHORT operand
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateUShortOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ USHORT operand, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateUShortOperandInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<COperandInstruction> pInstruction;
    pInstruction.Attach(new COperandInstruction(opcode, TRUE, sizeof(USHORT), (BYTE*)&operand));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateUShortOperandInstruction"));
    return S_OK;
}

// Create an instance of an instruction that takes an INT32 operand
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateIntOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ INT32 operand, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateULongOperandInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<COperandInstruction> pInstruction;
    pInstruction.Attach(new COperandInstruction(opcode, TRUE, sizeof(INT32), (BYTE*)&operand));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateULongOperandInstruction"));
    return S_OK;
}

// Create an instance of an instruction that takes a LONG operand
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateLongOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ INT64 operand, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateLongOperandInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<COperandInstruction> pInstruction;
    pInstruction.Attach(new COperandInstruction(opcode, TRUE, sizeof(INT64), (BYTE*)&operand));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateLongOperandInstruction"));
    return S_OK;
}


// Create an instance of an instruction that takes a float operand
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateFloatOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ float operand, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateLongOperandInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<COperandInstruction> pInstruction;
    pInstruction.Attach(new COperandInstruction(opcode, TRUE, sizeof(float), (BYTE*)&operand));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateLongOperandInstruction"));
    return S_OK;
}

// Create an instance of an instruction that takes a double operand
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateDoubleOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ double operand, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateDoubleOperandInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<COperandInstruction> pInstruction;
    pInstruction.Attach(new COperandInstruction(opcode, TRUE, sizeof(double), (BYTE*)&operand));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateDoubleOperandInstruction"));
    return S_OK;
}

// Create an instance of an instruction that takes a metadata token operand
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateTokenOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ mdToken operand, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateDoubleOperandInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<COperandInstruction> pInstruction;
    pInstruction.Attach(new COperandInstruction(opcode, TRUE, sizeof(mdToken), (BYTE*)&operand));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateDoubleOperandInstruction"));
    return S_OK;
}

// Create a branch instruction
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateBranchInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ IInstruction* pBranchTarget, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateBranchInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<CBranchInstruction> pInstruction;
    pInstruction.Attach(new CBranchInstruction(opcode, TRUE));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // Allow callers to initiliaze the branch target later
    if (pBranchTarget != NULL)
    {
        IfFailRet(pInstruction->SetBranchTarget(pBranchTarget));
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateBranchInstruction"));
    return S_OK;
}

// Create a switch instruction
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateSwitchInstruction(
    _In_ enum ILOrdinalOpcode opcode,
    _In_ DWORD cBranchTargets,
    _In_reads_(cBranchTargets) IInstruction** ppBranchTargets,
    _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateSwitchInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<CSwitchInstruction> pInstruction;
    pInstruction.Attach(new CSwitchInstruction(opcode, TRUE, cBranchTargets, ppBranchTargets));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateSwitchInstruction"));
    return S_OK;
}




// Creates an operand instruction of type Cee_Ldc_I4, or Cee_Ldc_I4_S, Cee_Ldc_I4_0, Cee_Ldc_I4_1, Cee_Ldc_I4_2...
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateLoadConstInstruction(_In_ int value, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateLoadConstInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<CLoadConstInstruction> pInstruction;
    pInstruction.Attach(new CLoadConstInstruction(value));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateLoadConstInstruction"));

    return S_OK;
}

// Creates an operand instruction of type Cee_Ldloc, or (Cee_Ldloc_S, (Cee_Ldloc_0, Cee_Ldloc_1, Cee_Ldloc_1...
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateLoadLocalInstruction(_In_ USHORT index, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateLoadLocalInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<CLoadLocalInstruction> pInstruction;
    pInstruction.Attach(new CLoadLocalInstruction(index));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateLoadLocalInstruction"));

    return S_OK;
}

// Creates an operand instruction of type Cee_Ldloca, or Cee_Ldloca_S
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateLoadLocalAddressInstruction(_In_ USHORT index, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateLoadLocalAddressInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<CLoadLocalAddrInstruction> pInstruction;
    pInstruction.Attach(new CLoadLocalAddrInstruction(index));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateLoadLocalAddressInstruction"));

    return S_OK;
}

// Creates an operand instruction of type Cee_Stloc, or Cee_Stloc_S, Cee_Stloc_0, Cee_Stloc_1, Cee_Stloc_2...
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateStoreLocalInstruction(_In_ USHORT index, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateStoreLocalInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<CStoreLocalInstruction> pInstruction;
    pInstruction.Attach(new CStoreLocalInstruction(index));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateStoreLocalInstruction"));

    return S_OK;
}

// Creates an operand instruction of type Cee_Ldarg, or Cee_Ldarg_S, Cee_Ldarg_0, Cee_Ldarg_1, Cee_Ldarg_2...
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateLoadArgInstruction(_In_ USHORT index, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateLoadArgInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<CLoadArgInstruction> pInstruction;
    pInstruction.Attach(new CLoadArgInstruction(index));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateLoadArgInstruction"));

    return S_OK;
}

// Creates an operand instruction of type Cee_Ldarga or Cee_Ldarga_S
HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::CreateLoadArgAddressInstruction(_In_ USHORT index, _Out_ IInstruction** ppInstruction)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CInstructionFactory::CreateLoadArgAddressInstruction"));
    IfNullRetPointer(ppInstruction);

    CComPtr<CLoadArgAddrInstruction> pInstruction;
    pInstruction.Attach(new CLoadArgAddrInstruction(index));
    if (pInstruction == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppInstruction = (IInstruction*)(pInstruction.p);
    (*ppInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstructionFactory::CreateLoadArgAddressInstruction"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstructionFactory::DecodeInstructionByteStream(_In_ DWORD cbMethod, _In_ LPCBYTE instructionBytes, _Out_ IInstructionGraph** ppInstructionGraph)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstructionFactory::DecodeInstructionByteStream"));
    IfNullRetPointer(instructionBytes);
    IfNullRetPointer(ppInstructionGraph);
    *ppInstructionGraph = NULL;

    CComPtr<CInstructionGraph> pInstructionGraph;
    pInstructionGraph.Attach(new CInstructionGraph());

    LPCBYTE pEndOfCode = instructionBytes + cbMethod;

    IfFailRet(pInstructionGraph->DecodeInstructions(instructionBytes, pEndOfCode));

    CComPtr<CInstruction> pCurrInstruction;
    pInstructionGraph->GetFirstInstruction((IInstruction**)(&pCurrInstruction));

    // The instruction graph will set these up as if they were from existing il.
    // However, the intended use case of DecodeInstructionByteStream is for parsing a
    // blob of il to be inserted into another il graph. So, mark all of these
    // instructions "New" and set the original offset to 0.
    while (pCurrInstruction != NULL)
    {
        IfFailRet(pCurrInstruction->SetOriginalOffset(0));
        IfFailRet(pCurrInstruction->SetInstructionGeneration(InstructionGeneration::Generation_New));

        CComPtr<IInstruction> pTemp = (IInstruction*)pCurrInstruction;
        pCurrInstruction.Release();
        pTemp->GetNextInstruction((IInstruction**)(&pCurrInstruction));
    }

    *ppInstructionGraph = (IInstructionGraph*)(pInstructionGraph.Detach());

    CLogging::LogMessage(_T("End CInstructionFactory::DecodeInstructionByteStream"));
    return hr;
}
