// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "DataContainer.h"
#include "ModuleInfo.h"
#include "InstructionGraph.h"

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class CInstructionGraph;
    class CInstruction;

    class __declspec(uuid("4FAE37AE-766E-4908-A7D6-4A723F1F2CBD"))
    CExceptionClause : public IExceptionClause, public CDataContainer
    {
    private:
        CorExceptionFlag m_flags;
        CComPtr<CInstruction> m_pTryFirstInstruction;
        CComPtr<CInstruction> m_pTryLastInstruction;
        CComPtr<CInstruction> m_pHandlerFirstInstruction;
        CComPtr<CInstruction> m_pHandlerLastInstruction;
        CComPtr<CInstruction> m_pFilterFirstInstruction;

        mdToken m_ExceptionHandlerType;

        // Non addref'd back pointer to method info
        CMethodInfo* m_pMethodInfo;

    public:
        CExceptionClause(CMethodInfo* m_pMethodInfo);

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CExceptionClause);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CExceptionClause);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IExceptionClause*>(this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }

    public:

        // The instruction graph is passed in directly rather than being obtained from the method info
        // because during the diagnostic dumping logic, the graph will be from the rendered function
        // body and not directly from the method info.
        HRESULT InitializeFromSmall(
            _In_ const IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_SMALL* pSmallClause,
            _In_ CInstructionGraph* pInstructionGraph
            );
        HRESULT InitializeFromFat(
            _In_ const IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* _pFatClause,
            _In_ CInstructionGraph* pInstructionGraph
            );

        HRESULT RenderExceptionClause(_In_ IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* pEHClause);

        // Search the instruction pointers for pInstructionOld and update pointers
        HRESULT UpdateInstruction(_In_ CInstruction* pInstructionOld, _In_ CInstruction* pInstructionNew);

    // IExceptionClause methods
    public:
        virtual HRESULT __stdcall GetFlags(_Out_ DWORD* pFlags) override;
        virtual HRESULT __stdcall GetTryFirstInstruction(_Out_ IInstruction** ppInstruction) override;
        virtual HRESULT __stdcall GetTryLastInstruction(_Out_ IInstruction** ppInstruction) override;
        virtual HRESULT __stdcall GetHandlerFirstInstruction(_Out_ IInstruction** ppInstruction) override;
        virtual HRESULT __stdcall GetHandlerLastInstruction(_Out_ IInstruction** ppInstruction) override;
        virtual HRESULT __stdcall GetFilterFirstInstruction(_Out_ IInstruction** ppInstruction) override;
        // The token of the type in the catch block
        virtual HRESULT __stdcall GetExceptionHandlerType(_Out_ mdToken* pToken) override;

        // flags is CorExceptionFlag
        virtual HRESULT __stdcall SetFlags(_In_ DWORD flags) override;
        virtual HRESULT __stdcall SetTryFirstInstruction(_In_ IInstruction* pInstruction) override;
        virtual HRESULT __stdcall SetTryLastInstruction(_In_ IInstruction* pInstruction) override;
        virtual HRESULT __stdcall SetHandlerFirstInstruction(_In_ IInstruction* pInstruction) override;
        virtual HRESULT __stdcall SetHandlerLastInstruction(_In_ IInstruction* pInstruction) override;
        virtual HRESULT __stdcall SetFilterFirstInstruction(_In_ IInstruction* pInstruction) override;
        // The token of the type in the catch block
        virtual HRESULT __stdcall SetExceptionHandlerType(_In_ mdToken token) override;
    };
}