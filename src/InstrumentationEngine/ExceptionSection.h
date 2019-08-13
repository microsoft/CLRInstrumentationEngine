// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "DataContainer.h"
#include "ModuleInfo.h"
#include "InstructionGraph.h"
#include "Instruction.h"
#include "ExceptionClause.h"

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class CMethodInfo;
    class CInstructionGraph;
    class CExceptionClause;

    class __declspec(uuid("CA62A6B1-878B-4AC0-A0A0-F42147CD8E33"))
    CExceptionSection : public IExceptionSection, public CDataContainer
    {
    private:
        CRITICAL_SECTION m_cs;

        // Non-addref'd back pointer to method info
        CMethodInfo* m_pMethodInfo;
        vector<CComPtr<IExceptionClause>> m_exceptionClauses;

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CExceptionSection);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CExceptionSection);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IExceptionSection*>(this),
                static_cast<IDataContainer*>(this),
                riid,
                ppvObject
                );
        }

    public:
        CExceptionSection(_In_ CMethodInfo* pMethodInfo);
        ~CExceptionSection();

        // The instruction graph is passed in directly rather than being obtained from the method info
        // because during the diagnostic dumping logic, the graph will be from the rendered function
        // body and not directly from the method info.
        HRESULT Initialize(
            _In_ const IMAGE_COR_ILMETHOD* pMethodHeader,
            _In_ ULONG cbMethodSize,
            _In_ CInstructionGraph* pInstructionGraph
            );

        HRESULT CreateExceptionHeader(_Out_ BYTE** ppExceptionHeader, _Out_ DWORD* pcbExceptionHeader);

        // Search the exception clauses for pointers to pInstructionOld and update pointers
        HRESULT UpdateInstruction(_In_ CInstruction* pInstructionOld, _In_ CInstruction* pInstructionNew);

    public:
        virtual HRESULT __stdcall GetMethodInfo(_Out_ IMethodInfo** ppMethodInfo) override;
        virtual HRESULT __stdcall GetExceptionClauses(_Out_ IEnumExceptionClauses** ppEnumExceptionClauses) override;
        virtual HRESULT __stdcall AddExceptionClause(_In_ IExceptionClause* pExceptionClause) override;
        virtual HRESULT __stdcall RemoveExceptionClause(_In_ IExceptionClause* pExceptionClause) override;
        virtual HRESULT __stdcall RemoveAllExceptionClauses() override;

        virtual HRESULT __stdcall AddNewExceptionClause(
            _In_ DWORD flags,
            _In_ IInstruction* pTryFirstInstruction,
            _In_ IInstruction* pTryLastInstruction,
            _In_ IInstruction* pHandlerFirstInstruction,
            _In_ IInstruction* pHandlerLastInstruction,
            _In_ IInstruction* pFilterLastInstruction,
            _In_ mdToken handlerTypeToken,
            _Out_ IExceptionClause** ppExceptionClause
            ) override;

    private:
        HRESULT FindExceptionClauseInsertionPoint(
            _In_ CExceptionClause* pNewExceptionClause,
            _Out_ vector<CComPtr<IExceptionClause>>::iterator& iter
            );

        // Returns true if the range for pExistingExceptionClause
        // is contained within the range for pNewExceptionClause
        // Called by FindExceptionClauseInsertionPoint to find the correct insertion point
        // for a new exception clause.
         HRESULT IsExistingTryContainedWithinNewTry(
            _In_ CExceptionClause* pExistingExceptionClause,
            _In_ CExceptionClause* pNewExceptionClause,
            _Out_ bool* pbIsContained
            );
    };
}

