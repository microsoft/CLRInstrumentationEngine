// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

struct ILOpcodeInfo
{
public:
    LPCTSTR m_name;    // Display name of the opcode
    DWORD m_opcodeLength;    // Length of the opcode (1 or 2)
    DWORD m_operandLength; // Length of the parameter length
    ILOperandType m_type;  // The type of the operand
    ILOrdinalOpcode m_alternate;  // The index of the alternate short/long form of the instruction
    ILOpcodeFlags m_flags;
    DWORD m_popSlots;             // Number of slots this instruction pops off the instruction graph
    DWORD m_pushSlots;            // Number of slots this instruction pushes on the instruction graph
};

enum InstrumentationType
{
    InsertBefore,
    InsertAfter,
    Replace,
    Remove,
    RemoveAll
};

class CInstrumentInstruction
{
private:
    DWORD m_dwOffset;
    ILOrdinalOpcode m_opcode;
    ILOpcodeInfo m_opcodeInfo;

    // only supporting integer operands for the time being.
    INT64 m_operand;

    InstrumentationType m_instrType;

    DWORD m_dwRepeatCount;

public:
    CInstrumentInstruction(DWORD dwOffset, ILOrdinalOpcode opcode, ILOpcodeInfo& opcodeInfo, INT64 operand, InstrumentationType instrType, DWORD dwRepeatCount)
    {
        m_dwOffset = dwOffset;
        m_opcode = opcode;
        m_opcodeInfo = opcodeInfo;
        m_operand = operand;
        m_instrType = instrType;
        m_dwRepeatCount = dwRepeatCount;
    }

    HRESULT EmitIL(vector<BYTE>& buffer)
    {
        if ((m_opcode == ILOrdinalOpcode::Cee_Switch) || (m_opcodeInfo.m_flags == ILOpcodeFlag_Branch))
        {
            return E_NOTIMPL;
        }
        if (m_opcodeInfo.m_opcodeLength == 2)
        {
            buffer.push_back(0xFE);
        }
        buffer.push_back((BYTE)m_opcode);

        if (m_opcodeInfo.m_operandLength != 0)
        {
            size_t index = buffer.size();
            buffer.resize(buffer.size() + m_opcodeInfo.m_operandLength);
            memcpy_s(&buffer[index], m_opcodeInfo.m_operandLength, (BYTE*)&m_operand, m_opcodeInfo.m_operandLength);
        }
        return S_OK;
    }

    DWORD GetOffset()
    {
        return m_dwOffset;
    }

    ILOrdinalOpcode GetOpcode()
    {
        return m_opcode;
    }

    ILOpcodeInfo GetOpcodeInfo()
    {
        return m_opcodeInfo;
    }

    INT64 GetOperand()
    {
        return m_operand;
    }

    InstrumentationType GetInstrumentationType()
    {
        return m_instrType;
    }

    DWORD GetRepeatCount()
    {
        return m_dwRepeatCount;
    }
};

class CLocalType
{
    private:
        std::wstring m_typeName;
    public:
        CLocalType(const std::wstring& typeName) : m_typeName(typeName)
        {
        }
        const std::wstring& GetTypeName() const
        {
            return m_typeName;
        }
};

struct CInstrumentMethodPointTo
{
    std::wstring m_assemblyName;
    std::wstring m_typeName;
    std::wstring m_methodName;
};

struct CInjectAssembly
{
    std::wstring m_sourceAssemblyName;
    std::wstring m_targetAssemblyName;
};


class CInstrumentMethodEntry
{
private:
    CComBSTR m_bstrModuleName;
    CComBSTR m_bstrMethodName;
    BOOL m_bIsRejit;
    BOOL m_isReplacement;
    BOOL m_bMakeSingleRetFirst;
    BOOL m_bMakeSingleRetLast;
    BOOL m_bAddExceptionHandler;

    vector<shared_ptr<CInstrumentInstruction>> m_instructions;
    vector<COR_IL_MAP> m_baselineMap;
    vector<CLocalType> m_locals;
    shared_ptr<CInstrumentMethodPointTo> m_pointTo;
public:
    CInstrumentMethodEntry(BSTR bstrModuleName, BSTR bstrMethodName, BOOL isRejit, BOOL makeSingleRetFirst, BOOL makeSingleRetLast, BOOL addExceptionHandler)
        : m_pointTo(nullptr),
        m_bstrModuleName(bstrModuleName),
        m_bstrMethodName(bstrMethodName),
        m_bIsRejit(isRejit),
        m_isReplacement(FALSE),
        m_bMakeSingleRetFirst(makeSingleRetFirst),
        m_bMakeSingleRetLast(makeSingleRetLast),
        m_bAddExceptionHandler(addExceptionHandler)
    {
    }

    HRESULT GetModuleName(BSTR* pbstrModuleName)
    {
        m_bstrModuleName.CopyTo(pbstrModuleName);
        return S_OK;
    }

    HRESULT GetMethodName(BSTR* pbstrMethodName)
    {
        m_bstrMethodName.CopyTo(pbstrMethodName);
        return S_OK;
    }

    HRESULT GetIsRejit(BOOL* pbValue)
    {
        *pbValue = m_bIsRejit;
        return S_OK;
    }

    HRESULT AddLocals(const vector<CLocalType>& locals)
    {
        m_locals = locals;
        return S_OK;
    }

    HRESULT AddInstrumentInstructions(vector<shared_ptr<CInstrumentInstruction>>& instructions)
    {
        m_instructions = instructions;
        return S_OK;
    }

    HRESULT AddCorILMap(const vector<COR_IL_MAP>& ilMap)
    {
        m_baselineMap = ilMap;
        return S_OK;
    }

    void SetPointTo(std::shared_ptr<CInstrumentMethodPointTo>& pointTo)
    {
        m_pointTo = pointTo;
    }

    const vector<COR_IL_MAP>& GetCorILMap()
    {
        return m_baselineMap;
    }

    vector<shared_ptr<CInstrumentInstruction>> GetInstructions()
    {
        return m_instructions;
    }

    const vector<CLocalType>& GetLocals()
    {
        return m_locals;
    }

    void SetReplacement(BOOL isReplacement)
    {
        m_isReplacement = isReplacement;
    }

    BOOL IsReplacement()
    {
        return m_isReplacement;
    }

    BOOL IsSingleRetFirst()
    {
        return m_bMakeSingleRetFirst;
    }

    BOOL IsSingleRetLast()
    {
        return m_bMakeSingleRetLast;
    }

    BOOL IsAddExceptionHandler()
    {
        return m_bAddExceptionHandler;
    }

    std::shared_ptr<CInstrumentMethodPointTo>& GetPointTo()
    {
        return m_pointTo;
    }

};