// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "DataContainer.h"
#include "MethodInfo.h"

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class CMethodInfo;
    class CInstruction;

    class __declspec(uuid("8494B179-BB72-4C7D-A33A-3F0C6F0D9F9D"))
    CInstructionGraph : public IInstructionGraph, public CDataContainer
    {
        CRITICAL_SECTION m_cs;

        // non-addref'd pointer to method info.
        CMethodInfo* m_pMethodInfo;

        CComPtr<CInstruction> m_pFirstInstruction;
        CComPtr<CInstruction> m_pLastInstruction;

        CComPtr<CInstruction> m_pUninstrumentedFirstInstruction;
        CComPtr<CInstruction> m_pUninstrumentedLastInstruction;

        CComPtr<CInstruction> m_pOrigFirstInstruction;
        CComPtr<CInstruction> m_pOrigLastInstruction;

        // Cor il map that maps the original instructions to the baseline instructions. This is
        // set during a call to CInstructionGraph::CreateBaseline and does not change as the graph
        // changes.
        std::vector<COR_IL_MAP> m_originalToBaselineIlMap;

        // Map from the baseline to the instrumented graph. This is what is actually given to the clr.
        std::vector<COR_IL_MAP> m_baselineIlMap;

        // True if CreateBaseline has been called by an instrumentation method
        bool m_bHasBaselineBeenSet;

        bool m_bAreInstructionsStale;

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CInstructionGraph);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CInstructionGraph);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IInstructionGraph*>(this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }
    public:
        CInstructionGraph();
        ~CInstructionGraph();

        HRESULT Initialize(_In_ CMethodInfo* pMethodInfo);
        HRESULT DecodeInstructions(_In_ LPCBYTE pCodeBase, _In_ LPCBYTE pEndOfCode);

        HRESULT EncodeIL(
            _Out_ CAutoVectorPtr<BYTE>* ppILBuffer,
            _Out_ DWORD* pdwILStreamLen,
            _Out_ CAutoVectorPtr<COR_IL_MAP>* ppCorILMap,
            _Out_ DWORD* pdwCorILMapmLen
            );

        HRESULT CalculateInstructionOffsets();

        HRESULT GetInstructionAtOffset(_In_ DWORD offset, _Out_ CInstruction** ppInstruction);
        HRESULT GetInstructionAtEndOffset(_In_ DWORD offset, _Out_ CInstruction** ppInstruction);

        HRESULT CalculateMaxStack(_Out_ DWORD* pMaxStack);

        constexpr CInstruction* FirstInstructionInternal() { return m_pFirstInstruction.p; }
        constexpr CInstruction* OriginalFirstInstructionInternal() { return m_pOrigFirstInstruction.p; }
        HRESULT RefreshInstructions();

        // IInstructionGraph methods
    public:
        virtual HRESULT __stdcall GetMethodInfo(_Out_ IMethodInfo** ppMethodInfo) override;

        // Get the current first and last instructions reflecting changes that instrumentation methods have made
        virtual HRESULT __stdcall GetFirstInstruction(_Out_ IInstruction** ppInstruction) override;
        virtual HRESULT __stdcall GetLastInstruction(_Out_ IInstruction** ppInstruction) override;

        virtual HRESULT __stdcall GetOriginalFirstInstruction(_Out_ IInstruction** ppInstruction) override;
        virtual HRESULT __stdcall GetOriginalLastInstruction(_Out_ IInstruction** ppInstruction) override;

        virtual HRESULT __stdcall GetUninstrumentedFirstInstruction(_Out_ IInstruction** ppInstruction) override;
        virtual HRESULT __stdcall GetUninstrumentedLastInstruction(_Out_ IInstruction** ppInstruction) override;

        virtual HRESULT __stdcall GetInstructionAtOffset(_In_  DWORD offset, _Out_ IInstruction** ppInstruction) override;
        virtual HRESULT __stdcall GetInstructionAtOriginalOffset(_In_  DWORD offset, _Out_ IInstruction** ppInstruction) override;
        virtual HRESULT __stdcall GetInstructionAtUninstrumentedOffset(_In_ DWORD dwOffset, _Out_ IInstruction** ppInstruction) override;

        // Insert an instruction before another instruction. jmp offsets that point to the original instruction
        // are not updated to reflect this change and will now point to the new instruction
        virtual HRESULT __stdcall InsertBefore(_In_ IInstruction* pInstructionOrig, _In_ IInstruction* pInstructionNew) override;

        // Insert an instruction after another instruction. jmp offsets that point to the next instruction after
        // the other instruction are not updated to reflect this change
        virtual HRESULT __stdcall InsertAfter(_In_ IInstruction* pInstructionOrig, _In_ IInstruction* pInstructionNew) override;

        // Insert an instruction before another instruction AND update branch offsets and exception ranges that used
        // to point to the old instruction to point to the new instruction.
        virtual HRESULT __stdcall InsertBeforeAndRetargetOffsets(_In_ IInstruction* pInstructionOrig, _In_ IInstruction* pInstructionNew) override;

        // Replace an instruction with another instruction. The old instruction continues to live in the original graph but is marked replaced
        virtual HRESULT __stdcall Replace(_In_ IInstruction* pInstructionOrig, _In_ IInstruction *pInstructionNew) override;

        // Remove an instruction with another instruction. The old instruction continues to live in the original graph but is marked deleted
        virtual HRESULT __stdcall Remove(_In_ IInstruction *pInstructionOrig) override;

        // Remove all instructions from the current graph. The original instructions are still accessible from the original first and original last
        // methods. These are all marked deleted  and their original next fields are still set.
        virtual HRESULT __stdcall RemoveAll() override;

        virtual HRESULT __stdcall CreateBaseline(
            _In_ LPCBYTE pCodeBase,
            _In_ LPCBYTE pEndOfCode,
            _In_ DWORD originalToBaselineCorIlMapSize,
            _In_ COR_IL_MAP originalToBaselineCorIlMap[],
            _In_ DWORD baselineSequencePointSize,
            _In_ DWORD baselineSequencePointList[]
            ) override;

        // true if CreateBaseline has previously been called.
        virtual HRESULT __stdcall HasBaselineBeenSet(_Out_ BOOL* pHasBaselineBeenSet) override;

        // Converts short form branch instructions to long form instructions.
        virtual HRESULT __stdcall ExpandBranches() override;

    private:
        HRESULT IsFirstInstructionInCatch(_In_ IInstruction* pInstr, _Out_ bool* pIsFirstInstructionInCatch);
        void MarkInstructionsStale() { m_bAreInstructionsStale = true; }
    };
}