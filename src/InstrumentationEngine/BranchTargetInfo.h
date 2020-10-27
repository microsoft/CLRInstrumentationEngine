// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once


namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("0E23A44A-6700-4810-889C-8067638C512E"))
        CBranchTargetInfo : public IUnknown, virtual CModuleRefCount
    {
    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CBranchTargetInfo);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CBranchTargetInfo);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void** ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IUnknown*>(this),
                this,
                riid,
                ppvObject
            );
        }
    public:
        static HRESULT GetInstance(_In_ CInstruction* pInstruction, _Outptr_ CBranchTargetInfo** ppResult);
        static HRESULT GetOrCreateInstance(_In_ CInstruction* pInstruction, _Out_ CBranchTargetInfo** ppResult);
        static HRESULT SetBranchTarget(_In_ CInstruction* pBranch, _In_ CInstruction* pNewTarget, _In_opt_ CInstruction* pOldTarget);
        static HRESULT RetargetBranches(_In_ CInstruction* pOriginalInstruction, _In_ CInstruction* pNewInstruction);
        void Disconnect();

        virtual ~CBranchTargetInfo() = default;
    private:

        // This is a raw pointer to avoid circular references.
        CInstruction* m_pInstruction;

        // Using raw pointers for speed. The graph should keep these pointers
        // alive.
        std::unordered_set<CInstruction*> m_branches;

        CBranchTargetInfo(_In_ CInstruction* pInstruction);
        HRESULT Retarget(_In_ CInstruction* pNewInstruction);
        void Remove(_In_ CInstruction* pOldBranch);
    };
}