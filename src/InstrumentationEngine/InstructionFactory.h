// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Instruction.h"

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("4FAE37AE-766E-4908-A7D6-4A723F1F2CBD"))
    CInstructionFactory : public IInstructionFactory, public CModuleRefCount
    {
    private:


    public:
        CInstructionFactory();

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CInstructionFactory);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CInstructionFactory);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IInstructionFactory*>(this),
                riid,
                ppvObject
                );
        }

    public:
        // Create an instance of an instruction that takes no operands
        virtual HRESULT __stdcall CreateInstruction(_In_ enum ILOrdinalOpcode opcode, _Out_ IInstruction** ppInstruction) override;

        // Create an instance of an instruction that takes a byte operand
        virtual HRESULT __stdcall CreateByteOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ BYTE operand, _Out_ IInstruction** ppInstruction) override;

        // Create an instance of an instruction that takes a USHORT operand
        virtual HRESULT __stdcall CreateUShortOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ USHORT operand, _Out_ IInstruction** ppInstruction) override;

        // Create an instance of an instruction that takes an INT operand
        virtual HRESULT __stdcall CreateIntOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ INT32 operand, _Out_ IInstruction** ppInstruction) override;

        // Create an instance of an instruction that takes a LONG operand
        virtual HRESULT __stdcall CreateLongOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ INT64 operand, _Out_ IInstruction** ppInstruction) override;

        // Create an instance of an instruction that takes a float operand
        virtual HRESULT __stdcall CreateFloatOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ float operand, _Out_ IInstruction** ppInstruction) override;

        // Create an instance of an instruction that takes a double operand
        virtual HRESULT __stdcall CreateDoubleOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ double operand, _Out_ IInstruction** ppInstruction) override;

        // Create an instance of an instruction that takes a metadata token operand
        virtual HRESULT __stdcall CreateTokenOperandInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ mdToken operand, _Out_ IInstruction** ppInstruction) override;

        // Create a branch instruction
        virtual HRESULT __stdcall CreateBranchInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ IInstruction* pBranchTarget, _Out_ IInstruction** ppInstruction) override;

        // Create a switch instruction
        virtual HRESULT __stdcall CreateSwitchInstruction(_In_ enum ILOrdinalOpcode opcode, _In_ DWORD cBranchTargets, _In_reads_(cBranchTargets) IInstruction** ppBranchTargets, _Out_ IInstruction** ppInstruction) override;


        // Helpers that adjust the opcode to match the operand size

        // Creates an operand instruction of type Cee_Ldc_I4, or Cee_Ldc_I4_S, Cee_Ldc_I4_0, Cee_Ldc_I4_1, Cee_Ldc_I4_2...
        virtual HRESULT __stdcall CreateLoadConstInstruction(_In_ int value, _Out_ IInstruction** ppInstruction) override;

        // Creates an operand instruction of type Cee_Ldloc, or (Cee_Ldloc_S, (Cee_Ldloc_0, Cee_Ldloc_1, Cee_Ldloc_1...
        virtual HRESULT __stdcall CreateLoadLocalInstruction(_In_ USHORT index, _Out_ IInstruction** ppInstruction) override;

        // Creates an operand instruction of type Cee_Ldloca, or Cee_Ldloca_S
        virtual HRESULT __stdcall CreateLoadLocalAddressInstruction(_In_ USHORT index, _Out_ IInstruction** ppInstruction) override;

        // Creates an operand instruction of type Cee_Stloc, or Cee_Stloc_S, Cee_Stloc_0, Cee_Stloc_1, Cee_Stloc_2...
        virtual HRESULT __stdcall CreateStoreLocalInstruction(_In_ USHORT index, _Out_ IInstruction** ppInstruction) override;

        // Creates an operand instruction of type Cee_Ldarg, or Cee_Ldarg_S, Cee_Ldarg_0, Cee_Ldarg_1, Cee_Ldarg_2...
        virtual HRESULT __stdcall CreateLoadArgInstruction(_In_ USHORT index, _Out_ IInstruction** ppInstruction) override;

        // Creates an operand instruction of type Cee_Ldarga or Cee_Ldarga_S
        virtual HRESULT __stdcall CreateLoadArgAddressInstruction(_In_ USHORT index, _Out_ IInstruction** ppInstruction) override;


        // Given a byte stream that points to MSIL, create new graph of IInstructions that represents
        // that stream.
        //
        // 1) Each instruction in this stream will be marked as "New" and will not have the original
        //    next and original previous fields set.
        //
        // 2) This graph will not have a method info set and cannot create exception sections or handlers.
        virtual HRESULT __stdcall  DecodeInstructionByteStream(_In_ DWORD cbMethod, _In_ LPCBYTE instructionBytes, _Out_ IInstructionGraph** ppInstructionGraph) override;
    };
}