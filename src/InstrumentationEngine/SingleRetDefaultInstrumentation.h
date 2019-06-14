// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "InstrumentationEngine.h"

namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("570E6094-B0E1-465D-9AC8-31B7DE0AFEAC"))
    CSingleRetDefaultInstrumentation : public ISingleRetDefaultInstrumentation2, public CModuleRefCount
    {
    private:
        CComPtr<IInstructionGraph> m_pInstructionGraph;
        CComPtr<IInstruction> m_pJumpTarget;

    public:
        CSingleRetDefaultInstrumentation();
        virtual ~CSingleRetDefaultInstrumentation();

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CSingleRetDefaultInstrumentation);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CSingleRetDefaultInstrumentation);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<ISingleRetDefaultInstrumentation*>(this),
                static_cast<ISingleRetDefaultInstrumentation2*>(this),
                riid,
                ppvObject
                );
        }

    public:
        virtual HRESULT __stdcall Initialize(_In_ IInstructionGraph* pInstructionGraph) override;

        virtual HRESULT __stdcall ApplySingleRetDefaultInstrumentation() override;

        virtual HRESULT __stdcall GetBranchTargetInstruction(_Out_ IInstruction** ppInstruction) override;

    private:

        HRESULT GetReplaceableRets(_In_ vector<CComPtr<IInstruction>>& retInstructions);
    };
}