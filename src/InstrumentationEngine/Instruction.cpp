// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "Instruction.h"

HRESULT MicrosoftInstrumentationEngine::CInstruction::LogInstruction(_In_ BOOL ignoreTest)
{
    HRESULT hr = S_OK;

    tstring ignoreTestPrefix = ignoreTest ? _T("[TestIgnore]") : _T("");

    CLogging::LogDumpMessage(_T("%sIL_%04x %s"), ignoreTestPrefix.c_str(), m_offset, s_ilOpcodeInfo[m_opcode].m_name);

    CComPtr<IOperandInstruction> pOperand;
    if (SUCCEEDED(this->QueryInterface(__uuidof(IOperandInstruction), (LPVOID*)&pOperand)))
    {
        DWORD dwOperandSize = 0;
        IfFailRet(this->GetOperandLength(&dwOperandSize));

        if (dwOperandSize != 0)
        {
            CAutoVectorPtr<BYTE> pOperandValue;
            pOperandValue.Allocate(dwOperandSize);

            IfFailRet(pOperand->GetOperandValue(dwOperandSize, pOperandValue));

            ILOperandType operandType;
            IfFailRet(pOperand->GetOperandType(&operandType));

            switch(operandType)
            {
            case ILOperandType_Byte:
                CLogging::LogDumpMessage(_T("%s 0x%01x"), ignoreTestPrefix.c_str(), *(BYTE*)(pOperandValue.m_p));
                break;
            case ILOperandType_Int:
                CLogging::LogDumpMessage(_T("%s 0x%04x"), ignoreTestPrefix.c_str(), *(DWORD*)(pOperandValue.m_p));
                break;
            case ILOperandType_UShort:
                CLogging::LogDumpMessage(_T("%s 0x%02x"), ignoreTestPrefix.c_str(), *(USHORT*)(pOperandValue.m_p));
                break;
            case ILOperandType_Long:
                CLogging::LogDumpMessage(_T("%s 0x%08x"), ignoreTestPrefix.c_str(), *(LONGLONG*)(pOperandValue.m_p));
                break;
            case ILOperandType_Single:
                CLogging::LogDumpMessage(_T("%s 0x%f"), ignoreTestPrefix.c_str(), *(float*)(pOperandValue.m_p));
                break;
            case ILOperandType_Double:
                CLogging::LogDumpMessage(_T("%s 0x%f"), ignoreTestPrefix.c_str(), *(double*)(pOperandValue.m_p));
                break;
            case ILOperandType_Token:
                CLogging::LogDumpMessage(_T("%s 0x%04x"), ignoreTestPrefix.c_str(), *(DWORD*)(pOperandValue.m_p));
                break;
            default:
                CLogging::LogDumpMessage(_T("Invalid operand type"));
                break;
            }
        }
    }
    else
    {
        CComPtr<IBranchInstruction> pBranchInstruction;
        if (SUCCEEDED(this->QueryInterface(__uuidof(IBranchInstruction), (LPVOID*)&pBranchInstruction)))
        {
            CComPtr<IInstruction> pBranchTarget;
            IfFailRet(pBranchInstruction->GetBranchTarget(&pBranchTarget));

            DWORD targetOffset = 0;
            IfFailRet(pBranchTarget->GetOffset(&targetOffset));

            CLogging::LogDumpMessage(_T("%s IL_%04x"), ignoreTestPrefix.c_str(), targetOffset);
        }
        else
        {
            CComPtr<ISwitchInstruction> pSwitchInstruction;
            if (SUCCEEDED(this->QueryInterface(__uuidof(ISwitchInstruction), (LPVOID*)&pSwitchInstruction)))
            {
                DWORD branchCount = 0;
                IfFailRet(pSwitchInstruction->GetBranchCount(&branchCount));

                CLogging::LogDumpMessage(_T("%s (\r\n"), ignoreTestPrefix.c_str());

                for (DWORD i = 0; i < branchCount; i++)
                {
                    DWORD offset = 0;
                    IfFailRet(pSwitchInstruction->GetBranchOffset(i, &offset));

                    CLogging::LogDumpMessage(_T("%s IL_%04x"), ignoreTestPrefix.c_str(), offset);

                    if (i < branchCount - 1)
                    {
                        CLogging::LogDumpMessage(_T("%s,\r\n"), ignoreTestPrefix.c_str());
                    }
                }

                CLogging::LogDumpMessage(_T("%s)"), ignoreTestPrefix.c_str());
            }
        }
    }

    CLogging::LogDumpMessage(_T("%s\r\n"), ignoreTestPrefix.c_str());

    return hr;
}

//static
HRESULT MicrosoftInstrumentationEngine::CInstruction::InstructionFromBytes(
    _In_ LPCBYTE pCode,
    _In_ LPCBYTE pEndOfCode,
    _Out_ CInstruction** ppInstruction)
{
    HRESULT hr = S_OK;
    *ppInstruction = NULL;

    CLogging::LogMessage(_T("Starting CInstruction::InstructionFromBytes"));

    ILOrdinalOpcode opcode;
    CInstruction::OrdinalOpcodeFromBytes(pCode, pEndOfCode, &opcode);

    if ( opcode < 0 || opcode >= _countof(CInstruction::s_ilOpcodeInfo) )
    {
        CLogging::LogError(_T("CInstruction::InstructionFromBytes - incorrect il stream;"));
        return E_FAIL;
    }

    ILOpcodeFlags flags = CInstruction::s_ilOpcodeInfo[opcode].m_flags;
    ILOperandType type = CInstruction::s_ilOpcodeInfo[opcode].m_type;

    CComPtr<CInstruction> pInstruction;
    if (opcode == Cee_Switch)
    {
        CComPtr<CSwitchInstruction> pSwitchInstruction;
        pSwitchInstruction.Attach(new CSwitchInstruction(opcode, FALSE));
        if (!pSwitchInstruction)
        {
            return E_OUTOFMEMORY;
        }

        IfFailRet(pSwitchInstruction->InitializeFromBytes(pCode, pEndOfCode));
        *ppInstruction = pSwitchInstruction.Detach();
    }
    else
    {
        if ((flags & ILOpcodeFlag_Branch) != 0)
        {
            CComPtr<CBranchInstruction> pBranchInstruction;
            pBranchInstruction.Attach(new CBranchInstruction(opcode, FALSE));
            if (!pBranchInstruction)
            {
                return E_OUTOFMEMORY;
            }

            IfFailRet(pBranchInstruction->InitializeFromBytes(pCode, pEndOfCode));
            *ppInstruction = pBranchInstruction.Detach();
        }
        else
        {
            switch (type)
            {
                case ILOperandType_None:
                {
                    CComPtr<CInstruction> pNoneInstruction;
                    pNoneInstruction.Attach(new CInstruction(opcode, FALSE));
                    if (!pNoneInstruction)
                    {
                        return E_OUTOFMEMORY;
                    }

                    IfFailRet(pNoneInstruction->InitializeFromBytes(pCode, pEndOfCode));
                    *ppInstruction = pNoneInstruction.Detach();
                    break;
                }

                case ILOperandType_Byte:
                case ILOperandType_UShort:
                case ILOperandType_Int:
                case ILOperandType_Long:
                case ILOperandType_Single:
                case ILOperandType_Double:
                case ILOperandType_Token:
                {
                    CComPtr<COperandInstruction> pNumericOperandInstruction;
                    pNumericOperandInstruction.Attach(new COperandInstruction(opcode, FALSE));
                    if (!pNumericOperandInstruction)
                    {
                        return E_OUTOFMEMORY;
                    }

                    IfFailRet(pNumericOperandInstruction->InitializeFromBytes(pCode, pEndOfCode));
                    *ppInstruction = pNumericOperandInstruction.Detach();
                    break;
                }

                default:
                {
                    CLogging::LogMessage(_T("CInstruction::InstructionFromBytes - Incorrect operand type"));
                    return E_FAIL;
                }
            }
        }
    }

    CLogging::LogMessage(_T("End CInstruction::InstructionFromBytes"));

    return S_OK;
}


// static
HRESULT MicrosoftInstrumentationEngine::CInstruction::OrdinalOpcodeFromBytes(_In_ LPCBYTE pCode, _In_ LPCBYTE pEndOfCode, _Out_ ILOrdinalOpcode* pOpcode)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pOpcode);
    *pOpcode = (ILOrdinalOpcode)0;

    // check if the opcode is two bytes (thy're always prefixed with 0xFE)
    if(*pCode == 0xFE)
    {
        if(pCode + 1 >=  pEndOfCode)
        {
            CLogging::LogError(_T("CInstruction::OrdinalOpcodeFromBytes - Incorrect program"));
            return E_FAIL;
        }

        *pOpcode = (ILOrdinalOpcode)((ULONG)(0x100 | *(pCode + 1)));
        return S_OK;
    }

    *pOpcode = (ILOrdinalOpcode)(*pCode);

    return S_OK;
}

//virtual
HRESULT MicrosoftInstrumentationEngine::CInstruction::EmitIL(_In_ BYTE* pILBuffer, _In_ DWORD dwcbILBuffer)
{
    HRESULT hr = S_OK;
    ULONG curpos = m_offset;

    CInstruction::ILOpcodeInfo &opcodeInfo = CInstruction::s_ilOpcodeInfo [m_opcode];
    unsigned opcodeLength = opcodeInfo.m_opcodeLength;

    if (curpos + opcodeLength + opcodeInfo.m_operandLength > dwcbILBuffer)
    {
        CLogging::LogError(_T("ILInstruction::EmitIL"));
        return E_FAIL;
    }

    // If 2 bytes opcode then output also the first one
    if (m_opcode >= 0x100) // is it a short form ?
    {
        pILBuffer[curpos++] = 0xFE;
    }

    // Second opcode for long form or first for short form.
    pILBuffer[curpos++] = (BYTE) m_opcode;

    int operandSize = opcodeInfo.m_operandLength;

    bool bIsBranch = GetIsBranchInternal();
    bool bIsSwitch = GetIsSwitchInternal();

    // Finalize branch pointers
    if (bIsBranch)
    {
        CBranchInstruction* pBranch = (CBranchInstruction*)this;

        CComPtr<IInstruction> pTarget;
        IfFailRet(pBranch->GetBranchTarget(&pTarget));

        DWORD targetOffset;
        pTarget->GetOffset(&targetOffset);

        int nextOffset = m_offset + 1 + operandSize;

        int relativeBranchOffset = targetOffset - nextOffset;
        IfFailRetErrno(memcpy_s(pILBuffer + curpos, operandSize, &relativeBranchOffset, operandSize));
    }
    else if (bIsSwitch)
    {
        CSwitchInstruction* pSwitch = (CSwitchInstruction*)this;

        DWORD branchCount = 0;
        pSwitch->GetBranchCount(&branchCount);

        operandSize = (1 + branchCount) * sizeof (DWORD);
        int nextOffset = m_offset + 1 + operandSize;

        // Check the size of the buffer
        if (curpos + opcodeLength + operandSize > dwcbILBuffer)
        {
            CLogging::LogError(_T("ILInstruction::EmitIL"));
            return E_FAIL;
        }

        LPDWORD pOperands = (LPDWORD) (pILBuffer + curpos);

        // First element is the size
        pOperands[0] = branchCount;

        // Offset from the next instruction for all the 'case' statements
        for (DWORD i = 0; i < branchCount; i++)
        {
            CComPtr<IInstruction> pSwitchTargetInstruction;
            IfFailRet(pSwitch->GetBranchTarget(i, &pSwitchTargetInstruction));

            DWORD dwSwitchTargetOffset = 0;
            pSwitchTargetInstruction->GetOffset(&dwSwitchTargetOffset);

            pOperands[i + 1] = dwSwitchTargetOffset - nextOffset;
        }
    }
    else
    {
        if (operandSize != 0)
        {
            COperandInstruction* pOperandInstruction = (COperandInstruction*)(this);

            if (pILBuffer + curpos + operandSize <= pILBuffer + dwcbILBuffer)
            {
                pOperandInstruction->GetOperandValue(operandSize, (pILBuffer + curpos));
            }
            else
            {
                CLogging::LogError(_T("ILInstruction::EmitIL - buffer is too small for operands"));
                return E_FAIL;
            }
        }
    }

    return S_OK;
}

MicrosoftInstrumentationEngine::CInstruction::CInstruction(_In_ ILOrdinalOpcode opcode, _In_ BOOL isNew) :
    m_opcode(opcode),
    m_offset(0),
    m_origOffset(0),
    m_instructionGeneration(isNew ? InstructionGeneration::Generation_New : InstructionGeneration::Generation_Original),
    m_bIsRemoved(FALSE),
    m_pGraph(nullptr)
{
    DEFINE_REFCOUNT_NAME(CInstruction);
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::InitializeFromBytes(
    _In_ LPCBYTE pCode,
    _In_ LPCBYTE pEndOfCode
    )
{
    HRESULT hr = S_OK;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::SetIsRemoved()
{
    this->m_bIsRemoved = true;
    this->m_pGraph = nullptr;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetOffset(_Out_ DWORD* pdwOffset)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetOffset"));

    IfNullRetPointer(pdwOffset);
    IfFailRet(EnsureGraphUpdated());
    *pdwOffset = m_offset;

    CLogging::LogMessage(_T("End CInstruction::GetOffset"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetOriginalOffset(_Out_ DWORD* pdwOffset)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetOriginalOffset"));

    IfNullRetPointer(pdwOffset);
    IfFailRet(EnsureGraphUpdated());
    *pdwOffset = m_origOffset;

    CLogging::LogMessage(_T("End CInstruction::GetOriginalOffset"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetOpCodeName(_Out_ BSTR* pbstrName)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetOpCodeName"));

    IfNullRetPointer(pbstrName);

    CComBSTR bstrOpCodeName = s_ilOpcodeInfo[m_opcode].m_name;
    *pbstrName = bstrOpCodeName.Detach();

    CLogging::LogMessage(_T("End CInstruction::GetOpCodeName"));

    return S_OK;
}

 HRESULT MicrosoftInstrumentationEngine::CInstruction::GetOpCode(_Out_ ILOrdinalOpcode* pOpCode)
 {
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetOpCode"));
    IfNullRetPointer(pOpCode);

    *pOpCode = m_opcode;

    CLogging::LogMessage(_T("End CInstruction::GetOpCode"));

    return S_OK;
 }

// Only single byte opcodes have short and long forms
HRESULT MicrosoftInstrumentationEngine::CInstruction::GetAlternateOrdinalOpcode(_Out_ ILOrdinalOpcode* pAlternative)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetAlternateOrdinalOpcode"));
    IfNullRetPointer(pAlternative);

    *pAlternative = s_ilOpcodeInfo[m_opcode].m_alternate;

    CLogging::LogMessage(_T("End CInstruction::GetAlternateOrdinalOpcode"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetOpcodeFlags(_Out_ ILOpcodeFlags* pFlags)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetAlternateOrdinalOpcode"));
    IfNullRetPointer(pFlags);

    *pFlags = s_ilOpcodeInfo[m_opcode].m_flags;

    CLogging::LogMessage(_T("End CInstruction::GetAlternateOrdinalOpcode"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetInstructionLength(_Out_ DWORD* pdwLength)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetInstructionLength "));
    IfNullRetPointer(pdwLength);

    *pdwLength = s_ilOpcodeInfo[m_opcode].m_opcodeLength + s_ilOpcodeInfo[m_opcode].m_operandLength;

    CLogging::LogMessage(_T("End CInstruction::GetInstructionLength "));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetOpcodeLength(_Out_ DWORD* pdwLength)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetOpcodeLength"));
    IfNullRetPointer(pdwLength);

    *pdwLength = s_ilOpcodeInfo[m_opcode].m_opcodeLength;

    CLogging::LogMessage(_T("End CInstruction::GetOpcodeLength"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetOperandType(_Out_ ILOperandType* pType)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetOpcodeLength"));
    IfNullRetPointer(pType);

    *pType = s_ilOpcodeInfo[m_opcode].m_type;

    CLogging::LogMessage(_T("End CInstruction::GetOpcodeLength"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetOperandLength(_Out_ DWORD* pdwLength)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetOpcodeLength"));
    IfNullRetPointer(pdwLength);

    *pdwLength = s_ilOpcodeInfo[m_opcode].m_operandLength;

    CLogging::LogMessage(_T("End CInstruction::GetOpcodeLength"));

    return S_OK;
}

// set to true if this instruction did not exist in the original instruction stream.
HRESULT MicrosoftInstrumentationEngine::CInstruction::GetIsNew(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetIsNew"));
    IfNullRetPointer(pbValue);

    *pbValue = m_instructionGeneration == InstructionGeneration::Generation_New ? TRUE : FALSE;

    CLogging::LogMessage(_T("End CInstruction::GetIsNew"));

    return S_OK;
}

// set to true of this instruction has been replaced in the instruction stream.
HRESULT MicrosoftInstrumentationEngine::CInstruction::GetIsRemoved(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetIsRemoved"));
    IfNullRetPointer(pbValue);

    *pbValue = m_bIsRemoved;

    CLogging::LogMessage(_T("End CInstruction::GetIsRemoved"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetInstructionSize(_Out_ DWORD* pdwSize)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetInstructionSize"));
    IfNullRetPointer(pdwSize);

    DWORD dwOpCodeLength = 0;
    IfFailRet(this->GetOpcodeLength(&dwOpCodeLength));

    DWORD dwOperandLength = 0;
    IfFailRet(this->GetOperandLength(&dwOperandLength));

    *pdwSize = dwOpCodeLength + dwOperandLength;

    CLogging::LogMessage(_T("End CInstruction::GetInstructionSize"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetIsBranch(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetIsBranch"));
    IfNullRetPointer(pbValue);

    *pbValue = GetIsBranchInternal();

    CLogging::LogMessage(_T("End CInstruction::GetIsBranch"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetIsSwitch(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetIsSwitch"));
    IfNullRetPointer(pbValue);

    *pbValue = GetIsSwitchInternal();

    CLogging::LogMessage(_T("End CInstruction::GetIsSwitch"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetIsCallInstruction(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetIsCallInstruction"));
    IfNullRetPointer(pbValue);

    *pbValue = (m_opcode == Cee_Call || m_opcode == Cee_Calli || m_opcode == Cee_Callvirt);

    CLogging::LogMessage(_T("End CInstruction::GetIsCallInstruction"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetTerminationType(_Out_ InstructionTerminationType* pTerminationType)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pTerminationType);

    *pTerminationType = TerminationType_FallThrough;

    // The termination type information comes from InternalApis\Clr\inc\opcode.def.
    switch (m_opcode)
    {
    case Cee_Break:
        *pTerminationType = TerminationType_Trap;
        break;

    case Cee_Switch:
        *pTerminationType = TerminationType_Switch;
        break;

    case Cee_Ret:
    case Cee_Endfinally:
    case Cee_Endfilter:
        *pTerminationType = TerminationType_Return;
        break;

    case Cee_Call:
    case Cee_Jmp:
    case Cee_Newobj:
        *pTerminationType = TerminationType_Call;
        break;

    case Cee_Calli:
    case Cee_Callvirt:
        *pTerminationType = TerminationType_IndirectCall;
        break;

    case Cee_Br_S:
    case Cee_Br:
    case Cee_Leave:
    case Cee_Leave_S:
        *pTerminationType = TerminationType_Branch;
        break;

    case Cee_Throw:
    case Cee_Rethrow:
        *pTerminationType = TerminationType_Throw;
        break;

    default:
        if (m_opcode >= Cee_Brfalse_S && m_opcode < Cee_Switch)
        {
            *pTerminationType = TerminationType_ConditionalBranch;
        }
        else
        {
            // For the rest, use fallthrough.
            *pTerminationType = TerminationType_FallThrough;
        }
    }


    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetIsFallThrough(_Out_ BOOL* pbIsFallThrough)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pbIsFallThrough);
    *pbIsFallThrough = false;

    InstructionTerminationType terminationType;
    IfFailRet(GetTerminationType(&terminationType));

    if (terminationType == TerminationType_FallThrough)
    {
        *pbIsFallThrough = TRUE;
    }
    else
    {
        *pbIsFallThrough = FALSE;
    }

    return hr;
}

// Get the current next and previous instructions reflecting changes that instrumentation methods have made
HRESULT MicrosoftInstrumentationEngine::CInstruction::GetNextInstruction(_Out_ IInstruction** ppNextInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetNextInstruction"));
    IfNullRetPointer(ppNextInstruction);
    *ppNextInstruction = NULL;

    if (m_pNextInstruction == NULL)
    {
        CLogging::LogMessage(_T("CInstruction::GetNextInstruction - no next instruction"));
        return E_FAIL;
    }

    *ppNextInstruction = (IInstruction*)(m_pNextInstruction.p);
    (*ppNextInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstruction::GetNextInstruction"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetPreviousInstruction(_Out_ IInstruction** ppPrevInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetPreviousInstruction"));
    IfNullRetPointer(ppPrevInstruction);
    *ppPrevInstruction = NULL;

    if (m_pPreviousInstruction == NULL)
    {
        CLogging::LogMessage(_T("CInstruction::GetPreviousInstruction - no previous instruction"));
        return E_FAIL;
    }

    *ppPrevInstruction = (IInstruction*)(m_pPreviousInstruction.p);
    (*ppPrevInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstruction::GetPreviousInstruction"));

    return S_OK;
}

// Get the original instructions from the graph before any instrumentation method made chanages to it.
HRESULT MicrosoftInstrumentationEngine::CInstruction::GetOriginalNextInstruction(_Out_ IInstruction** ppNextInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetOriginalNextInstruction"));
    IfNullRetPointer(ppNextInstruction);
    *ppNextInstruction = NULL;

    if (m_pOriginalNextInstruction == NULL)
    {
        CLogging::LogMessage(_T("CInstruction::GetOriginalNextInstruction - no original next instruction"));
        return E_FAIL;
    }

    *ppNextInstruction = (IInstruction*)(m_pOriginalNextInstruction.p);
    (*ppNextInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstruction::GetOriginalNextInstruction"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetOriginalPreviousInstruction(_Out_ IInstruction** ppPrevInstruction)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::GetOriginalPreviousInstruction"));
    IfNullRetPointer(ppPrevInstruction);
    *ppPrevInstruction = NULL;

    if (m_pOriginalPreviousInstruction == NULL)
    {
        CLogging::LogMessage(_T("CInstruction::GetOriginalPreviousInstruction - no original previous instruction"));
        return E_FAIL;
    }

    *ppPrevInstruction = (IInstruction*)(m_pOriginalPreviousInstruction.p);
    (*ppPrevInstruction)->AddRef();

    CLogging::LogMessage(_T("End CInstruction::GetOriginalPreviousInstruction"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::SetNextInstruction(_In_opt_ CInstruction* pInstruction, _In_ bool setOrig)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::SetNextInstruction"));

    m_pNextInstruction = pInstruction;

    if (m_pOriginalNextInstruction == NULL && setOrig)
    {
        m_pOriginalNextInstruction = m_pNextInstruction;
    }

    CLogging::LogMessage(_T("End CInstruction::SetNextInstruction"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::SetPreviousInstruction(_In_opt_ CInstruction* pInstruction, _In_ bool setOrig)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::SetPreviousInstruction"));

    m_pPreviousInstruction = pInstruction;

    if (m_pOriginalPreviousInstruction == NULL && setOrig)
    {
        m_pOriginalPreviousInstruction = m_pPreviousInstruction;
    }

    CLogging::LogMessage(_T("End CInstruction::SetPreviousInstruction"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::SetOriginalOffset(_In_ ULONG offset)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::SetOriginalOffset offset is %04x"), offset);

    m_origOffset = offset;

    CLogging::LogMessage(_T("End CInstruction::SetOriginalOffset"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::SetOffset(_In_ ULONG offset)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::SetOffset offset is %04x"), offset);

    m_offset = offset;

    CLogging::LogMessage(_T("End CInstruction::SetOffset"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::SetInstructionGeneration(_In_ InstructionGeneration instructionGeneration)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CInstruction::SetInstructionGeneration"));

    m_instructionGeneration = instructionGeneration;

    CLogging::LogMessage(_T("End CInstruction::SetInstructionGeneration"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetInstructionGeneration(_Out_ InstructionGeneration* pInstructionGeneration)
{
    IfNullRetPointer(pInstructionGeneration);
    *pInstructionGeneration = m_instructionGeneration;
    return S_OK;
}

MicrosoftInstrumentationEngine::COperandInstruction::COperandInstruction(
        _In_ ILOrdinalOpcode opcode,
        _In_ BOOL isNew,
        _In_ DWORD cbBytes,
        _In_ BYTE* pBytes
        ) : CInstruction(opcode, isNew)
{
    if (s_ilOpcodeInfo[m_opcode].m_operandLength != cbBytes)
    {
        CLogging::LogError(_T("COperandInstruction::COperandInstruction = Incorrect length passed for operand"));
        return;
    }

    SetOperandValue(s_ilOpcodeInfo[m_opcode].m_operandLength, pBytes);
}

MicrosoftInstrumentationEngine::COperandInstruction::COperandInstruction(
    _In_ ILOrdinalOpcode opcode,
    _In_ BOOL isNew
    )
    : CInstruction(opcode, isNew)
{
    memset(&m_value, 0, sizeof(m_value));
}

HRESULT MicrosoftInstrumentationEngine::COperandInstruction::InitializeFromBytes(
    _In_ LPCBYTE pCode,
    _In_ LPCBYTE pEndOfCode
    )
{
    HRESULT hr = S_OK;

    ULONG opcodeSize = s_ilOpcodeInfo[m_opcode].m_opcodeLength;

    LPCBYTE p = pCode + opcodeSize;

    if(p >= pEndOfCode)
    {
        CLogging::LogError(_T("COperandInstruction::Initialize - Invalid program"));
        return E_FAIL;
    }

    unsigned operandSize = s_ilOpcodeInfo[m_opcode].m_operandLength;
    if(p + operandSize > pEndOfCode)
    {
        CLogging::LogError(_T("COperandInstruction::Initialize - Invalid program"));
        return E_FAIL;
    }

    switch (s_ilOpcodeInfo[m_opcode].m_type)
    {
        case ILOperandType_Byte:
            m_value.b = *(BYTE*)p;
            break;
        case ILOperandType_UShort:
            m_value.u = *(USHORT*)p;
            break;
        case ILOperandType_Int:
            m_value.i = *(ULONG*)p;
            break;
        case ILOperandType_Long:
            m_value.l = *(LONGLONG*)p;
            break;
        case ILOperandType_Single:
            m_value.s = *(float*)p;
            break;
        case ILOperandType_Double:
            m_value.d = *(double*)p;
            break;
        case ILOperandType_Token:
            m_value.t = *(mdToken*)p;
            break;
        default:
        {
            CLogging::LogError(_T("COperandInstruction::Initialize - Invalid program"));
            return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::COperandInstruction::GetOperandType(_Out_ enum ILOperandType* pType)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting COperandInstruction::GetOperandType"));
    IfNullRetPointer(pType);

    *pType = s_ilOpcodeInfo[m_opcode].m_type;

    CLogging::LogMessage(_T("End COperandInstruction::GetOperandType"));

    return S_OK;
}


HRESULT MicrosoftInstrumentationEngine::COperandInstruction::GetOperandValue(
    _In_ DWORD dwSize,
    _Out_writes_(dwSize) BYTE* pBytes
    )
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting COperandInstruction::GetOperandValue"));
    IfNullRetPointer(pBytes);

    if (dwSize < s_ilOpcodeInfo[m_opcode].m_operandLength)
    {
        CLogging::LogError(_T("COperandInstruction::GetOperandValue buffer length is too small for operand"));
        return E_FAIL;
    }

    switch (s_ilOpcodeInfo[m_opcode].m_type)
    {
        case ILOperandType_Byte:
            *(BYTE*)(pBytes) = m_value.b;
            break;
        case ILOperandType_UShort:
            *(USHORT*)(pBytes) = m_value.u;
            break;
        case ILOperandType_Int:
            *(ULONG*)(pBytes) = m_value.i;
            break;
        case ILOperandType_Long:
            *(LONGLONG*)(pBytes) = m_value.l;
            break;
        case ILOperandType_Single:
            *(float*)(pBytes) = m_value.s;
            break;
        case ILOperandType_Double:
            *(double*)(pBytes) = m_value.d;
            break;
        case ILOperandType_Token:
            *(mdToken*)(pBytes) = m_value.t;
            break;
        default:
        {
            CLogging::LogError(_T("COperandInstruction::SetOperandValue - Invalid program"));
            return E_FAIL;
        }
    }

    CLogging::LogMessage(_T("End COperandInstruction::GetOperandValue"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::COperandInstruction::SetOperandValue(
        _In_ DWORD dwSize,
        _In_ BYTE* pBytes
        )
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting COperandInstruction::SetOperandValue"));
    IfNullRetPointer(pBytes);

    if (dwSize != s_ilOpcodeInfo[m_opcode].m_operandLength)
    {
        CLogging::LogError(_T("COperandInstruction::SetOperandValue - new value does not match size of operand"));
        return E_FAIL;
    }

    switch (s_ilOpcodeInfo[m_opcode].m_type)
    {
        case ILOperandType_Byte:
             m_value.b = *(BYTE*)(pBytes);
            break;
        case ILOperandType_UShort:
            m_value.u = *(USHORT*)(pBytes);
            break;
        case ILOperandType_Int:
            m_value.i = *(ULONG*)(pBytes);
            break;
        case ILOperandType_Long:
            m_value.l = *(LONGLONG*)(pBytes);
            break;
        case ILOperandType_Single:
            m_value.s = *(float*)(pBytes);
            break;
        case ILOperandType_Double:
            m_value.d = *(double*)(pBytes);
            break;
        case ILOperandType_Token:
            m_value.t = *(mdToken*)(pBytes);
            break;
        default:
        {
            CLogging::LogError(_T("COperandInstruction::SetOperandValue - Invalid program"));
            return E_FAIL;
        }
    }

    CLogging::LogMessage(_T("End COperandInstruction::SetOperandValue"));

    return S_OK;
}

MicrosoftInstrumentationEngine::CBranchInstruction::CBranchInstruction(
    _In_ ILOrdinalOpcode opcode,
    _In_ BOOL isNew
    )
    :  CInstruction(opcode, isNew),
    m_decodedTargetOffset(0)
{

}

HRESULT MicrosoftInstrumentationEngine::CBranchInstruction::InitializeFromBytes(
    _In_ LPCBYTE pCode,
    _In_ LPCBYTE pEndOfCode
    )
{
    HRESULT hr = S_OK;

    // Set this to the branch delta for now and fix it up when on the second pass through the instructions
    ULONG opcodeSize = CInstruction::s_ilOpcodeInfo[m_opcode].m_opcodeLength;

    ULONG* p = (ULONG*)(pCode + opcodeSize);
    if((LPCBYTE)p >= pEndOfCode)
    {
        CLogging::LogError(_T("COperandInstruction::Initialize - Invalid program"));
        return E_FAIL;
    }

    DWORD targetOffset = 0;

    BOOL bIsShortBranch;
    IfFailRet(IsShortBranch(&bIsShortBranch));

    if(bIsShortBranch)
    {
        // add opcodesize and size of target offset (1 byte) to the offset itself
        // to have the exact target offset based on the beginning of this instruction
        char c = *(char *)p;
        targetOffset = (int)c + opcodeSize + 1;
    }
    else
    {
        // add opcodesize and size of target offset (4 bytes) to the offset itself
        // to have the exact target offset based on the beginning of this instruction
        targetOffset = (*p) + opcodeSize + 4;
    }

    if((pCode + static_cast<int>(targetOffset)) >= pEndOfCode)
    {
        CLogging::LogError(_T("COperandInstruction::Initialize - Invalid program"));
        return E_FAIL;
    }

    m_decodedTargetOffset = targetOffset;

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CBranchInstruction::IsShortBranch(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CBranchInstruction::IsShortBranch"));
    IfNullRetPointer(pbValue);

    *pbValue = (m_opcode < Cee_Br || m_opcode == Cee_Leave_S);

    CLogging::LogMessage(_T("End CBranchInstruction::IsShortBranch"));

    return hr;
}


//Expand short branches to the long form. Note that this shifts offsets
HRESULT MicrosoftInstrumentationEngine::CBranchInstruction::ExpandBranch()
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CBranchInstruction::ExpandBranch"));

    if (CInstruction::s_ilOpcodeInfo[m_opcode].m_operandLength == 1)
    {
        //Expand the branch to the long form
        if (m_opcode == Cee_Leave_S)
        {
            m_opcode = Cee_Leave;
        }
        else
        {
            //VSASSERT(m_opcode >= Cee_Br_S && m_opcode <= Cee_Blt_Un_S, L"");
            m_opcode = (ILOrdinalOpcode) (m_opcode + (Cee_Br - Cee_Br_S));
        }
    }

    CLogging::LogMessage(_T("End CBranchInstruction::ExpandBranch"));

    return S_OK;
}


HRESULT MicrosoftInstrumentationEngine::CBranchInstruction::GetBranchTarget(_Out_ IInstruction** ppTarget)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CBranchInstruction::GetTargetOffset"));
    IfNullRetPointer(ppTarget);

    *ppTarget = (IInstruction*)m_pBranchTarget;
    (*ppTarget)->AddRef();

    CLogging::LogMessage(_T("End CBranchInstruction::GetTargetOffset"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CBranchInstruction::GetTargetOffset(_Out_ DWORD* pOffset)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CBranchInstruction::GetTargetOffset"));
    IfNullRetPointer(pOffset);
    *pOffset = 0;

    if (m_pBranchTarget != nullptr)
    {
        hr = m_pBranchTarget->GetOffset(pOffset);
    }
    else
    {
        *pOffset = m_decodedTargetOffset;
    }

    CLogging::LogMessage(_T("End CBranchInstruction::GetTargetOffset"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CBranchInstruction::SetBranchTarget(_In_ IInstruction* pInstruction)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CBranchInstruction::SetBranchTarget"));
    IfNullRetPointer(pInstruction);

    m_pBranchTarget = (CInstruction*)pInstruction;

    if (m_pBranchTarget == nullptr)
    {
        // The decoded branch target is now invalid.
        m_decodedTargetOffset = 0;
    }

    if (m_pOrigBranchTarget == NULL && m_instructionGeneration != InstructionGeneration::Generation_New)
    {
        m_pOrigBranchTarget = m_pBranchTarget;
    }

    CLogging::LogMessage(_T("End CBranchInstruction::SetBranchTarget"));

    return hr;
}


MicrosoftInstrumentationEngine::CSwitchInstruction::CSwitchInstruction(
        _In_ ILOrdinalOpcode opcode,
        _In_ BOOL isNew
        ) : CInstruction(opcode, isNew)
{

}

MicrosoftInstrumentationEngine::CSwitchInstruction::CSwitchInstruction(
        _In_ ILOrdinalOpcode opcode,
        _In_ BOOL isNew,
        _In_ DWORD cBranchTargets,
        _In_reads_(cBranchTargets) IInstruction** ppBranchTargets
        ) : CInstruction(opcode, isNew)
{

    m_branchTargets.reserve(cBranchTargets);

    for (DWORD i = 0; i < cBranchTargets; i++)
    {
        CComPtr<CInstruction> pTarget = (CInstruction*)(ppBranchTargets[i]);
        m_branchTargets[i] = pTarget;
    }
}

HRESULT MicrosoftInstrumentationEngine::CSwitchInstruction::InitializeFromBytes(
    _In_ LPCBYTE pCode,
    _In_ LPCBYTE pEndOfCode
    )
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CSwitchInstruction::Initialize"));

    const ULONG* p = (ULONG*)(pCode + CInstruction::s_ilOpcodeInfo[m_opcode].m_opcodeLength);
    ULONG count = *p;  // First U4 integer is the count of branch deltas
    p++;

    // Prepare the branch array.
    // Note that the actual setting of the targets happens later while the instruction graph initializes
    m_branchTargets.reserve(count);
    m_origBranchTargetOffsets.reserve(count);

    for (DWORD i = 0; i < count; i++)
    {
        m_branchTargets.push_back(NULL);
        m_origBranchTargetOffsets.push_back(*p);
        p++;
    }

    CLogging::LogMessage(_T("End CSwitchInstruction::Initialize"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CSwitchInstruction::GetOperandLength(_Out_ DWORD* pdwSize)
{
    HRESULT hr = S_OK;

    // operand size is number of branch targets + 1 + size of dword for the branch target count
    *pdwSize = ((DWORD)(m_branchTargets.size()) + 1) * sizeof(DWORD);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CSwitchInstruction::GetBranchTarget(_In_ DWORD index, _Out_ IInstruction** ppTarget)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CSwitchInstruction::GetBranchTarget"));

    *ppTarget = GetBranchTargetInternal(index);
    if (*ppTarget != NULL)
    {
        (*ppTarget)->AddRef();
    }

    CLogging::LogMessage(_T("End CSwitchInstruction::GetBranchTarget"));

    return hr;
}

CInstruction* MicrosoftInstrumentationEngine::CSwitchInstruction::GetBranchTargetInternal(_In_ DWORD index)
{
    if (index >= m_branchTargets.size())
    {
        CLogging::LogError(_T("CSwitchInstruction::GetBranchTargetInternal - invalid index"));
        return NULL;
    }

    const CComPtr<CInstruction>& pInstruction = m_branchTargets[index];
    if (pInstruction == NULL)
    {
        CLogging::LogError(_T("CSwitchInstruction::GetBranchTarget - branch target at index is null"));
        return NULL;
    }

    return pInstruction.p;
}


HRESULT MicrosoftInstrumentationEngine::CSwitchInstruction::SetBranchTarget(_In_ DWORD index, _In_ IInstruction* pTarget)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CSwitchInstruction::SetBranchTarget"));

    m_branchTargets[index] = (CInstruction*)(pTarget);

    CLogging::LogMessage(_T("End CSwitchInstruction::SetBranchTarget"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CSwitchInstruction::RemoveBranchTargetAt(_In_ DWORD index)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CSwitchInstruction::RemoveBranchTargetAt"));

    m_branchTargets.erase(m_branchTargets.begin() + index);

    CLogging::LogMessage(_T("End CSwitchInstruction::RemoveBranchTargetAt"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CSwitchInstruction::RemoveBranchTarget(_In_ IInstruction* pTarget)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CSwitchInstruction::RemoveBranchTarget"));

    for(std::vector<CComPtr<CInstruction>>::iterator iter = m_branchTargets.begin(); iter != m_branchTargets.end(); iter++)
    {
        if(*iter == pTarget)
        {
            m_branchTargets.erase(iter);
        }
    }

    CLogging::LogMessage(_T("End CSwitchInstruction::RemoveBranchTarget"));


    return hr;
}



HRESULT MicrosoftInstrumentationEngine::CSwitchInstruction::ReplaceBranchTarget(_In_ IInstruction* pOriginal, _In_  IInstruction *pNew)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CSwitchInstruction::SetBranchTarget"));

    for(ULONG i = 0; i < m_branchTargets.size(); i++)
    {
        if(m_branchTargets[i] == pOriginal)
        {
            SetBranchTarget(i, pNew);
            //Need to continue as several branches of the switch might point to the same location
        }
    }

    CLogging::LogMessage(_T("End CSwitchInstruction::SetBranchTarget"));

    return hr;
}


HRESULT MicrosoftInstrumentationEngine::CSwitchInstruction::GetBranchCount(_Out_ DWORD* pBranchCount)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CSwitchInstruction::GetBranchCount"));

    IfNullRetPointer(pBranchCount);
    *pBranchCount = (DWORD)(m_branchTargets.size());

    CLogging::LogMessage(_T("End CSwitchInstruction::GetBranchCount"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CSwitchInstruction::GetBranchOffset(_In_ DWORD index, _In_ DWORD* pdwOffset)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CSwitchInstruction::GetBranchCount"));

    IfNullRetPointer(pdwOffset);

    CComPtr<CInstruction> pInstruction = m_branchTargets[index];
    if (pInstruction != NULL)
    {
        IfFailRet(pInstruction->GetOffset(pdwOffset));
    }
    else
    {
        // The offsets encoded in the instruction stream are relative to this instruction
        *pdwOffset = m_origBranchTargetOffsets[index];
    }

    CLogging::LogMessage(_T("End CSwitchInstruction::GetBranchCount"));

    return hr;
}

MicrosoftInstrumentationEngine::CLoadConstInstruction::CLoadConstInstruction(_In_ int value) : COperandInstruction(Cee_Ldc_I4, TRUE)
{
    CLogging::LogMessage(_T("Starting CLoadConstInstruction::CLoadConstInstruction"));

    // Check if the instruction can can be made a one byte
    // where the value of the constant is implied by the opcode
    if (value >= -1 && value <= 8)
    {
        m_opcode = (ILOrdinalOpcode) (value + Cee_Ldc_I4_0);
    }
    // or a 2 bytes
    else if (((DWORD) value & ~(0x7f)) == 0)
    {
        m_opcode = Cee_Ldc_I4_S;
        m_value.b = (BYTE) value;
    }
    else
    {
        m_value.i = value;
    }

    CLogging::LogMessage(_T("End CLoadConstInstruction::CLoadConstInstruction"));
}

MicrosoftInstrumentationEngine::CLoadLocalInstruction::CLoadLocalInstruction(
           _In_ USHORT index
            ) : COperandInstruction(Cee_Ldloc, TRUE)
{
    CLogging::LogMessage(_T("Starting CLoadLocalInstruction::CLoadLocalInstruction"));

    // Check if the instruction can can be made a one byte
    if (index < 4)
    {
        m_opcode = (ILOrdinalOpcode) (index + Cee_Ldloc_0);
    }
    // or a 2 bytes
    else if (index <= 0xff)
    {
        m_opcode = Cee_Ldloc_S;
        m_value.b = (BYTE) index;
    }
    else
    {
        m_opcode = Cee_Ldloc;
        m_value.i = index;
    }

    CLogging::LogMessage(_T("End CLoadLocalInstruction::CLoadLocalInstruction"));
}



MicrosoftInstrumentationEngine::CLoadLocalAddrInstruction::CLoadLocalAddrInstruction(
           _In_ USHORT index
            ) : COperandInstruction(Cee_Ldloca, TRUE)
{
    CLogging::LogMessage(_T("Starting CLoadLocalAddrInstruction::CLoadLocalAddrInstruction"));

    // Check if the instruction can can be made a one byte
    if (index <= 0xff)
    {
        m_opcode = Cee_Ldloca_S;
        m_value.b = (BYTE) index;
    }
    else
    {
        m_opcode = Cee_Ldloca;
        m_value.i = index;
    }

    CLogging::LogMessage(_T("End CLoadLocalAddrInstruction::CLoadLocalAddrInstruction"));
}

MicrosoftInstrumentationEngine::CStoreLocalInstruction::CStoreLocalInstruction(
        _In_ USHORT index
        ) : COperandInstruction(Cee_Stloc, TRUE)
{
    CLogging::LogMessage(_T("Starting CLoadLocalAddrInstruction::CStoreLocalInstruction"));

    // Check if the instruction can can be made a one byte
    if (index < 4)
    {
        m_opcode = (ILOrdinalOpcode) (index + Cee_Stloc_0);
    }
    // or a 2 bytes
    else if (index <= 0xff)
    {
        m_opcode = Cee_Stloc_S;
        m_value.b = (BYTE) index;
    }
    else
    {
        m_opcode = Cee_Stloc;
        m_value.i = index;
    }

    CLogging::LogMessage(_T("End CLoadLocalAddrInstruction::CStoreLocalInstruction"));
}


MicrosoftInstrumentationEngine::CLoadArgInstruction::CLoadArgInstruction(
           _In_ USHORT index
            ) : COperandInstruction(Cee_Ldarg, TRUE)
{
    CLogging::LogMessage(_T("Starting CLoadLocalAddrInstruction::CLoadArgInstruction"));

    // Check if the instruction can can be made a one byte
    if (index < 4)
    {
        m_opcode = (ILOrdinalOpcode) (index + Cee_Ldarg_0);
    }
    // or a 2 bytes
    else if (index <= 0xff)
    {
        m_opcode = Cee_Ldarg_S;
        m_value.b = (BYTE) index;
    }
    else
    {
        m_opcode = Cee_Ldarg;
        m_value.i = (BYTE) index;
    }

    CLogging::LogMessage(_T("End CLoadLocalAddrInstruction::CLoadArgInstruction"));
}

MicrosoftInstrumentationEngine::CLoadArgAddrInstruction::CLoadArgAddrInstruction(
           _In_ USHORT index
            ) : COperandInstruction(Cee_Ldarga, TRUE)
{
    CLogging::LogMessage(_T("Starting CLoadLocalAddrInstruction::CLoadArgAddrInstruction"));

    // Check if the instruction can can be 2 bytes
    if (index <= 0xff)
    {
        m_opcode = Cee_Ldarga_S;
        m_value.b = (BYTE) index;
    }
    else
    {
        m_opcode = Cee_Ldarga;
        m_value.i = (BYTE) index;
    }

    CLogging::LogMessage(_T("End CLoadLocalAddrInstruction::CLoadArgAddrInstruction"));
}

// Return the instruction's impact on the execution stack.
// This can be positive for a push and negative for a pop.
// For variable stack impact (call, etc...) it calculates the stack impact using metadata.
HRESULT MicrosoftInstrumentationEngine::CInstruction::GetStackImpact(_In_ IMethodInfo* pMethodInfo, _In_ DWORD currStackDepth, _Out_ int* pStackImpact)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pStackImpact);
    IfNullRetPointer(pMethodInfo);
    *pStackImpact = 0;

    int stackImpact = 0;

    if (m_opcode == Cee_Leave || m_opcode == Cee_Leave_S)
    {
        // The evaluation stack is automatically emptied by these instructions.
        stackImpact = -1 * currStackDepth;
    }
    else
    {
        // Var push is used for anything that has call semantics. Specifically, it means variable stack impact.
        if (s_ilOpcodeInfo[m_opcode].m_pushSlots == VarPush)
        {
            PCCOR_SIGNATURE pSig = nullptr;
            DWORD sigLength = 0;
            CorCallingConvention callConv;
            DWORD parameterCount = 0;
            CorElementType retTypeElementType;
            IfFailRet(GetSignatureInfoFromCallToken(
                pMethodInfo,
                &pSig,
                &sigLength,
                &callConv,
                &parameterCount,
                &retTypeElementType
                ));

            if (retTypeElementType != ELEMENT_TYPE_VOID)
            {
                // add the  return value slot
                stackImpact++;
            }
        }
        else
        {
            // All other push operations
            stackImpact += s_ilOpcodeInfo[m_opcode].m_pushSlots;
        }

        if (s_ilOpcodeInfo[m_opcode].m_popSlots == VarPop)
        {
            if (m_opcode == Cee_Call || m_opcode == Cee_Calli || m_opcode == Cee_Call || m_opcode == Cee_Callvirt || m_opcode == Cee_Newobj)
            {
                PCCOR_SIGNATURE pSig = nullptr;
                DWORD sigLength = 0;
                CorCallingConvention callConv;
                DWORD parameterCount = 0;
                CorElementType retTypeElementType;
                IfFailRet(GetSignatureInfoFromCallToken(
                    pMethodInfo,
                    &pSig,
                    &sigLength,
                    &callConv,
                    &parameterCount,
                    &retTypeElementType
                    ));

                stackImpact -= parameterCount;

                // Pop the "this" parameter if the method isn't static and the instruction isn't newobj/ldtoken/ldftn
                if (IsFlagSet(callConv, IMAGE_CEE_CS_CALLCONV_HASTHIS) && m_opcode != Cee_Newobj)
                {
                    stackImpact--;
                }
            }
            else if (m_opcode == Cee_Ret)
            {
                CComPtr<IType> pType;
                IfFailRet(pMethodInfo->GetReturnType(&pType));

                CorElementType retCorElementType;
                IfFailRet(pType->GetCorElementType(&retCorElementType));

                if (retCorElementType != ELEMENT_TYPE_VOID)
                {
                    stackImpact--;
                }
            }
            else
            {
                CLogging::LogError(_T("CInstruction::GetStackImpact - unexpected opcode fr varpop"));
            }
        }
        else
        {
            // all other stack operations
            stackImpact -= s_ilOpcodeInfo[m_opcode].m_popSlots;
        }
    }

    *pStackImpact = stackImpact;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::SetGraph(_In_ CInstructionGraph* pGraph)
{
    IfNullRet(pGraph);
    IfFalseRet(m_pGraph == nullptr, E_UNEXPECTED);
    m_pGraph = pGraph;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::GetSignatureInfoFromCallToken(
    _In_ IMethodInfo* pMethodInfo,
    _Out_ PCCOR_SIGNATURE* ppSig,
    _Out_ DWORD* pSigLength,
    _Out_ CorCallingConvention* pCallConv,
    _Out_ DWORD* pParameterCount,
    _Out_ CorElementType* pRetTypeElementType
    )
{
    HRESULT hr = S_OK;

    ILOperandType operandType;
    IfFailRet(GetOperandType(&operandType));

    if (operandType != ILOperandType_Token)
    {
        CLogging::LogError(_T("CInstruction::GetStackImpact - Unexpected il operand type"));
        return E_FAIL;
    }

    COperandInstruction* pOperandThis = (COperandInstruction*)this;
    mdToken callToken = mdTokenNil;
    IfFailRet(pOperandThis->GetOperandValue(sizeof(mdToken), (BYTE*)&callToken));

    CComPtr<IModuleInfo> pModuleInfo;
    IfFailRet(pMethodInfo->GetModuleInfo(&pModuleInfo));

    CComPtr<IMetaDataImport> pMetaDataImport;
    IfFailRet(pModuleInfo->GetMetaDataImport((IUnknown**)&pMetaDataImport));

    // if the callToken is a methodSpec, get the parent MethodDef/MethodRef
    // to later retrieve the actual method signature
    if (TypeFromToken(callToken) == mdtMethodSpec)
    {
        CComPtr<IMetaDataImport2> pMetaDataImport2;
        IfFailRet(pMetaDataImport->QueryInterface(IID_IMetaDataImport2, (LPVOID*)&pMetaDataImport2));

        mdToken parentCallToken = mdTokenNil;
        IfFailRet(pMetaDataImport2->GetMethodSpecProps(
            callToken,
            &parentCallToken,
            NULL,
            NULL
        ));

        callToken = parentCallToken;
    }

    PCCOR_SIGNATURE pSigUntouched = nullptr;
    PCCOR_SIGNATURE pSig = nullptr;
    ULONG sigLength = 0;

    // Get the signature for the method token
    if (TypeFromToken(callToken) == mdtMemberRef)
    {
        mdToken targetToken = mdTokenNil;

        IfFailRet(pMetaDataImport->GetMemberRefProps(
            callToken,
            nullptr,
            nullptr,
            0,
            nullptr,
            &pSig,
            &sigLength
            ));
    }
    else if (TypeFromToken(callToken) == mdtMethodDef)
    {
        IfFailRet(pMetaDataImport->GetMemberProps(
            callToken,
            nullptr,
            nullptr,
            0,
            nullptr,
            nullptr,
            &pSig,
            &sigLength,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
            ));
    }
    else if (TypeFromToken(callToken) == mdtSignature)
    {
        IfFailRet(pMetaDataImport->GetSigFromToken(
            callToken,
            &pSig,
            &sigLength
            ));
    }
    pSigUntouched = pSig;

    // get a method signature, which looks like this: "calling Conv" + "argument Cnt" + "return Type" + "arg1" + "arg2" + ...
    CorCallingConvention callConv = IMAGE_CEE_CS_CALLCONV_MAX; // = 0x10 - first invalid calling convention;
    pSig += CorSigUncompressData(pSig, (ULONG *)&callConv);
    if ((callConv == IMAGE_CEE_CS_CALLCONV_MAX) || (callConv == IMAGE_CEE_CS_CALLCONV_FIELD))
    {
        CLogging::LogError(_T("Unexpected calling convention on method"));
        return E_UNEXPECTED;
    }

    if (IsFlagSet(callConv, IMAGE_CEE_CS_CALLCONV_GENERIC))
    {
        DWORD dwGenericParameterCount;
        // get the generic argument count
        pSig += CorSigUncompressData(pSig, &dwGenericParameterCount);
    }

    ULONG parameterCount = 0;
    // get the argument count
    pSig += CorSigUncompressData(pSig, &parameterCount);

    CorElementType retTypeElementType;
    pSig += CorSigUncompressData(pSig, (ULONG *)&retTypeElementType);

    *ppSig = pSigUntouched;
    *pSigLength = sigLength;
    *pCallConv = callConv;
    *pParameterCount = parameterCount;
    *pRetTypeElementType = retTypeElementType;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CInstruction::Disconnect()
{
    if (m_pPreviousInstruction)
    {
        m_pPreviousInstruction.Release();
    }
    if (m_pOriginalPreviousInstruction)
    {
        m_pOriginalPreviousInstruction.Release();
    }
    if (m_pNextInstruction)
    {
        m_pNextInstruction.Release();
    }
    if (m_pOriginalNextInstruction)
    {
        m_pOriginalNextInstruction.Release();
    }
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CBranchInstruction::Disconnect()
{
    if (m_pBranchTarget)
    {
        m_pBranchTarget.Release();
    }
    if (m_pOrigBranchTarget)
    {
        m_pOrigBranchTarget.Release();
    }

    return CInstruction::Disconnect();
}

HRESULT MicrosoftInstrumentationEngine::CSwitchInstruction::Disconnect()
{
    m_branchTargets.clear();

    return CInstruction::Disconnect();
}
