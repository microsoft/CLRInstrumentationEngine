// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "DataContainer.h"
#include "InstructionGraph.h"

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("BEA964F2-5527-4F7C-B606-D8A1BD8CFB39"))
    CInstruction : public IInstruction, public CDataContainer
    {
        friend CInstructionGraph::~CInstructionGraph();

        // Static table of information about each opcode
        struct ILOpcodeInfo
        {
        public:
            LPCTSTR m_name;               // Display name of the opcode
            DWORD m_opcodeLength;         // Length of the opcode (1 or 2)
            DWORD m_operandLength;        // Length of the parameter length
            ILOperandType m_type;         // The type of the operand
            ILOrdinalOpcode m_alternate;  // The index of the alternate short/long form of the instruction
            ILOpcodeFlags m_flags;
            DWORD m_popSlots;             // Number of slots this instruction pops off the instruction graph
            DWORD m_pushSlots;            // Number of slots this instruction pushes on the instruction graph
        };

    protected:
        static ILOpcodeInfo s_ilOpcodeInfo[];

        // CONSIDER: Store the original length as well? The graph immediately changes short branches etc...

        ILOrdinalOpcode m_opcode;
        DWORD m_offset;
        DWORD m_origOffset;
        InstructionGeneration m_instructionGeneration;
        BOOL m_bIsRemoved;

        // The graph that owns this instruction. Not a CComPtr
        // so that we don't have circular references.
        CInstructionGraph* m_pGraph;

        CComPtr<CInstruction> m_pNextInstruction;
        CComPtr<CInstruction> m_pPreviousInstruction;

        CComPtr<CInstruction> m_pOriginalNextInstruction;
        CComPtr<CInstruction> m_pOriginalPreviousInstruction;

        //Private helper to break up circular dependencies between instructions.
        virtual HRESULT Disconnect();
    public:
        CInstruction(
            _In_ ILOrdinalOpcode opcode,
            _In_ BOOL isNew
        );

        virtual HRESULT InitializeFromBytes(
            _In_ LPCBYTE pCode,
            _In_ LPCBYTE pEndOfCode
            );

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CInstruction);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CInstruction);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IInstruction*>(this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }

    public:
        HRESULT SetIsRemoved();

        // Return the instruction's impact on the execution stack.
        // This can be positive for a push and negative for a pop.
        // For variable stack impact (call, etc...) it calculates the stack impact using metadata.
        HRESULT GetStackImpact(_In_ IMethodInfo* pMethodInfo, _In_ DWORD currStackDepth, _Out_ int* pStackImpact);
        HRESULT SetGraph(_In_ CInstructionGraph* pGraph);
        constexpr CInstructionGraph* GetGraph() { return m_pGraph; }

    public:
        virtual HRESULT __stdcall GetOffset(_Out_ DWORD* pdwOffset) override;
        virtual HRESULT __stdcall GetOriginalOffset(_Out_ DWORD* pdwOffset) override;

        virtual HRESULT __stdcall GetOpCodeName(_Out_ BSTR* pbstrName) override;

        virtual HRESULT __stdcall GetOpCode(_Out_ ILOrdinalOpcode* pOpCode) override;

        // Only single byte opcodes have short and long forms
        virtual HRESULT __stdcall GetAlternateOrdinalOpcode(_Out_ ILOrdinalOpcode* pdwAlternative) override;

        virtual HRESULT __stdcall GetInstructionLength(_Out_ DWORD* pdwLength) override;

        virtual HRESULT __stdcall GetOpcodeFlags(_Out_ ILOpcodeFlags* pFlags) override;

        virtual HRESULT __stdcall GetOpcodeLength(_Out_ DWORD* pdwLength) override;
        virtual HRESULT __stdcall GetOperandType(_Out_ ILOperandType* pType) override;
        virtual HRESULT __stdcall GetOperandLength(_Out_ DWORD* pdwLength) override;

        // set to true if this instruction did not exist in the original instruction stream.
        virtual HRESULT __stdcall GetIsNew(_Out_ BOOL* pbValue) override;

        // set to true of this instruction has been removed or replaced from in the instruction stream.
        virtual HRESULT __stdcall GetIsRemoved(_Out_ BOOL* pbValue) override;

        virtual HRESULT __stdcall GetInstructionSize(_Out_ DWORD* pdwSize);

        virtual HRESULT __stdcall GetIsBranch(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsSwitch(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsCallInstruction(_Out_ BOOL* pbValue) override;

        virtual HRESULT __stdcall GetTerminationType(_Out_ InstructionTerminationType* pTerminationType) override;

        virtual HRESULT __stdcall GetIsFallThrough(_Out_ BOOL* pbIsFallThrough) override;

        // Get the current next and previous instructions reflecting changes that instrumentation methods have made
        virtual HRESULT __stdcall GetNextInstruction(_Out_ IInstruction** ppNextInstruction) override;
        virtual HRESULT __stdcall GetPreviousInstruction(_Out_ IInstruction** ppPrevInstruction) override;

        // Get the original instructions from the graph before any instrumentation method made chanages to it.
        virtual HRESULT __stdcall GetOriginalNextInstruction(_Out_ IInstruction** ppNextInstruction) override;
        virtual HRESULT __stdcall GetOriginalPreviousInstruction(_Out_ IInstruction** ppPrevInstruction) override;

        virtual HRESULT __stdcall GetInstructionGeneration(_Out_ InstructionGeneration* pInstructionGeneration) override;

    public:
        // NOTE: these do not fixup the rest of hte graph. Call the versions on instruction graph if you want
        // previous, and next to be udpated correctly
        HRESULT SetNextInstruction(_In_opt_ CInstruction* pInstruction, _In_ bool setOrig);
        HRESULT SetPreviousInstruction(_In_opt_ CInstruction* pInstruction, _In_ bool setOrig);


        HRESULT SetOriginalOffset(_In_ ULONG offset);
        HRESULT SetOffset(_In_ ULONG offset);
        HRESULT SetInstructionGeneration(_In_ InstructionGeneration instructionGeneration);

        static HRESULT InstructionFromBytes(_In_ LPCBYTE pCode, _In_ LPCBYTE pEndOfCode, _Out_ CInstruction** ppInstruction);
        static HRESULT OrdinalOpcodeFromBytes(_In_ LPCBYTE pCode, _In_ LPCBYTE pEndOfCode, _Out_ ILOrdinalOpcode* pOpcode);


        HRESULT EmitIL(_In_ BYTE* pILBuffer, _In_ DWORD dwcbILBuffer);

        HRESULT LogInstruction(_In_ BOOL ignoreTest);

        constexpr CInstruction* NextInstructionInternal() { return m_pNextInstruction.p; }
        constexpr CInstruction* PreviousInstructionInternal() { return m_pPreviousInstruction.p; }
        constexpr CInstruction* OriginalNextInstructionInternal() { return m_pOriginalNextInstruction.p; }
        constexpr CInstruction* OriginalPreviousInstructionInternal() { return m_pOriginalPreviousInstruction.p; }
        constexpr bool GetIsSwitchInternal() { return m_opcode == Cee_Switch; }
        bool GetIsBranchInternal() { return IsFlagSet(s_ilOpcodeInfo[m_opcode].m_flags, ILOpcodeFlag_Branch); }

    private:
        // If the callee token is a MethodSpec (generic method), the parameter count and signature returned
        // will represent the parent method declaration/reference, not the generic parameters or generic signature.
        HRESULT GetSignatureInfoFromCallToken(
            _In_ IMethodInfo* pMethodInfo,
            _Out_ PCCOR_SIGNATURE* ppSig,
            _Out_ DWORD* pSigLength,
            _Out_ CorCallingConvention* pCallConv,
            _Out_ DWORD* pParameterCount,
            _Out_ CorElementType* pRetTypeElementType
            );

        HRESULT EnsureGraphUpdated()
        {
            if (m_pGraph != nullptr)
            {
                return m_pGraph->RefreshInstructions();
            }

            return S_OK;
        }
    };

    class __declspec(uuid("D154FE63-8B15-465E-BE17-1C7B9AE8872B"))
    COperandInstruction : public IOperandInstruction, public CInstruction
    {
    protected:
        union ILOperandValue
        {
            BYTE b;
            ULONG i;
            USHORT u;
            ULONGLONG l;
            float s;
            double d;
            mdToken t;
        } m_value;

    public:
        COperandInstruction(
            _In_ ILOrdinalOpcode opcode,
            _In_ BOOL isNew,
            _In_ DWORD cbBytes,
            _In_ BYTE* pBytes
            );

        COperandInstruction(
            _In_ ILOrdinalOpcode opcode,
            _In_ BOOL isNew
            );

        virtual HRESULT InitializeFromBytes(
            _In_ LPCBYTE pCode,
            _In_ LPCBYTE pEndOfCode
            ) override;

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(COperandInstruction);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(COperandInstruction);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IOperandInstruction*>(this),
                static_cast<IInstruction*>(this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }

        // IOperandInstruction methods
    public:
        virtual HRESULT __stdcall GetOperandType(_Out_ enum ILOperandType* pType) override;

        virtual HRESULT __stdcall GetOperandValue(
            _In_ DWORD dwSize,
            _Out_writes_(dwSize) BYTE* pBytes
            ) override;

        // Size of operand must match expected size from operand type
        virtual HRESULT __stdcall SetOperandValue(
            _In_ DWORD dwSize,
            _In_ BYTE* pBytes
            ) override;
    };

    class __declspec(uuid("6BF2ED56-2775-49D6-918A-B483082B638B"))
    CLoadConstInstruction : public COperandInstruction
    {
    public:
        CLoadConstInstruction(
           _In_ int value
            );

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CLoadConstInstruction);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CLoadConstInstruction);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IOperandInstruction*>((COperandInstruction*)this),
                static_cast<IInstruction*>((CInstruction*)this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }
    };

    class __declspec(uuid("F75298AB-CCB6-4FE3-90A0-7A1199177843"))
    CLoadLocalInstruction : public COperandInstruction
    {
    public:
       CLoadLocalInstruction(
           _In_ USHORT index
            );

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CLoadConstInstruction);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CLoadConstInstruction);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IOperandInstruction*>((COperandInstruction*)this),
                static_cast<IInstruction*>((CInstruction*)this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }
    };

    class __declspec(uuid("A98F9127-B1AE-40DF-9DB3-EF7F27705A38"))
    CLoadLocalAddrInstruction : public COperandInstruction
    {
    public:
        CLoadLocalAddrInstruction(
           _In_ USHORT index
            );

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CLoadLocalAddrInstruction);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CLoadLocalAddrInstruction);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IOperandInstruction*>((COperandInstruction*)this),
                static_cast<IInstruction*>((CInstruction*)this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }
    };


    class __declspec(uuid("D57D18D3-4F45-4525-97C6-28C42B412E6F"))
    CStoreLocalInstruction : public COperandInstruction
    {
    public:
        CStoreLocalInstruction(
           _In_ USHORT index
            );

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CStoreLocalInstruction);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CStoreLocalInstruction);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IOperandInstruction*>((COperandInstruction*)this),
                static_cast<IInstruction*>((CInstruction*)this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }
    };

    class __declspec(uuid("622C6DBC-959F-4D0A-98F9-EB44EA84D8A7"))
    CLoadArgInstruction : public COperandInstruction
    {
    public:
        CLoadArgInstruction(
           _In_ USHORT index
            );

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CLoadArg);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CLoadArg);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IOperandInstruction*>((COperandInstruction*)this),
                static_cast<IInstruction*>((CInstruction*)this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }
    };

    class __declspec(uuid("ABEF9E96-9828-418F-8610-590F8AACE1DD"))
    CLoadArgAddrInstruction : public COperandInstruction
    {
    public:
       CLoadArgAddrInstruction(
           _In_ USHORT index
            );

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CLoadArgAddrInstruction);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CLoadArgAddrInstruction);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IOperandInstruction*>((COperandInstruction*)this),
                static_cast<IInstruction*>((CInstruction*)this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }
    };

    class __declspec(uuid("F30B9E10-A3DF-4F70-A0C4-7BAA9C6D4249"))
    CBranchInstruction : public IBranchInstruction, public CInstruction
    {
    protected:
        // NOTE: This is the target offset for the original decoded instruction.
        // It should not be used if m_pBranchTarget has been set.
        DWORD m_decodedTargetOffset;
        CComPtr<CInstruction> m_pBranchTarget;
        CComPtr<CInstruction> m_pOrigBranchTarget;

    public:
        CBranchInstruction(
            _In_ ILOrdinalOpcode opcode,
            _In_ BOOL isNew
            );

        virtual HRESULT InitializeFromBytes(
            _In_ LPCBYTE pCode,
            _In_ LPCBYTE pEndOfCode
            ) override;

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CILBranch);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CILBranch);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IBranchInstruction*>(this),
                static_cast<IInstruction*>((CInstruction*)this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }

        // IBranchInstruction methods
    public:
        virtual HRESULT __stdcall  IsShortBranch(_Out_ BOOL* pbValue) override;

        //Expand the branch to the long form
        virtual HRESULT __stdcall ExpandBranch() override;

        virtual HRESULT __stdcall GetBranchTarget(_Out_ IInstruction** ppTarget) override;

        virtual HRESULT __stdcall GetTargetOffset(_Out_ DWORD* pOffset) override;

        virtual HRESULT __stdcall SetBranchTarget(_In_ IInstruction* pInstruction) override;

        // Internal optimized helpers.
    public:
        constexpr CInstruction* GetBranchTargetInternal() { return m_pBranchTarget.p; }

    protected:
        virtual HRESULT Disconnect() override;
    };

    class __declspec(uuid("CB7CE31D-1134-44CD-8E9B-2A7DB6010616"))
    CSwitchInstruction : public ISwitchInstruction, public CInstruction
    {
    protected:

        // The branch target offsets as read out of the byte stream. This is used to
        // find the real target instructions during InitializeFromBytes
        std::vector<DWORD> m_origBranchTargetOffsets;

        // Vector of actual branch targets. Offsets etc... should be read from here
        // after the initialization phase is over.
        std::vector<CComPtr<CInstruction>> m_branchTargets;

    public:
        CSwitchInstruction(
            _In_ ILOrdinalOpcode opcode,
            _In_ BOOL isNew
            );

        CSwitchInstruction(
            _In_ ILOrdinalOpcode opcode,
            _In_ BOOL isNew,
            _In_ DWORD cBranchTargets,
            _In_reads_(cBranchTargets) IInstruction** ppBranchTargets
            );

        virtual HRESULT InitializeFromBytes(
            _In_ LPCBYTE pCode,
            _In_ LPCBYTE pEndOfCode
            ) override;

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CSwitchInstruction);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CSwitchInstruction);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<ISwitchInstruction*>(this),
                static_cast<IInstruction*>((CInstruction*)this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }

    public:
        virtual HRESULT __stdcall GetOperandLength(_Out_ DWORD* pdwLength) override;

        // ISwitchInstruction methods
    public:
        virtual HRESULT __stdcall GetBranchTarget(_In_ DWORD index, _Out_ IInstruction** ppTarget) override;

        virtual HRESULT __stdcall SetBranchTarget(_In_ DWORD index, _In_ IInstruction* pTarget) override;
        virtual HRESULT __stdcall RemoveBranchTargetAt(_In_ DWORD index) override;
        virtual HRESULT __stdcall RemoveBranchTarget(_In_ IInstruction* pTarget) override;

        virtual HRESULT __stdcall ReplaceBranchTarget(_In_ IInstruction* pOriginal, _In_  IInstruction *pNew) override;

        virtual HRESULT __stdcall GetBranchCount(_Out_ DWORD* pBranchCount) override;
        virtual HRESULT __stdcall GetBranchOffset(_In_ DWORD index, _In_ DWORD* pdwOffset) override;

        // Optimized helpers
    public:
        CInstruction* GetBranchTargetInternal(_In_ DWORD index);

    protected:
        virtual HRESULT Disconnect() override;

    };

    __declspec(selectany) CInstruction::ILOpcodeInfo CInstruction::s_ilOpcodeInfo[] =
    {
#ifndef PLATFORM_UNIX
#define OPDEF(ord, code, name, opcodeLen, operandLen, type, alt, flags, pop, push) { name, static_cast<DWORD>(opcodeLen), static_cast<DWORD>(operandLen), ##type, alt, flags, static_cast<DWORD>(pop), static_cast<DWORD>(push) },
#else
#define OPDEF(ord, code, name, opcodeLen, operandLen, type, alt, flags, pop, push) { name, static_cast<DWORD>(opcodeLen), static_cast<DWORD>(operandLen), type, alt, flags, static_cast<DWORD>(pop), static_cast<DWORD>(push) },
#endif
        #include "ILOpcodes.h"
        #undef OPDEF
    };
}
