#include "stdafx.h"
#include "refcount.h"
#include "Instruction.h"
#include "BranchTargetInfo.h"

namespace MicrosoftInstrumentationEngine
{
    CBranchTargetInfo::CBranchTargetInfo(_In_ CInstruction* pInstruction) : m_pInstruction(pInstruction)
    {

    }

    HRESULT CBranchTargetInfo::GetInstance(_In_ CInstruction* pInstruction, _Outptr_ CBranchTargetInfo** ppResult)
    {
        IfNullRet(pInstruction);
        IfNullRet(ppResult);

        *ppResult = nullptr;

        const GUID* uuid = &__uuidof(CBranchTargetInfo);
        CComPtr<IUnknown> pUnknown;
        HRESULT hr;
        // Don't assert, it is common for there to not be target info.
        IfFailRetNoLog(pInstruction->GetDataItem(uuid, uuid, &pUnknown));
        CComPtr<CBranchTargetInfo> pTargetInfo;
        IfFailRet(pUnknown.QueryInterface(&pTargetInfo));

        IfFalseRet(pTargetInfo != nullptr, E_UNEXPECTED);
        *ppResult = pTargetInfo.Detach();

        return S_OK;
    }

    HRESULT CBranchTargetInfo::GetOrCreateInstance(_In_ CInstruction* pInstruction, _Out_ CBranchTargetInfo** ppResult)
    {
        IfNullRet(pInstruction);
        IfNullRet(ppResult);
        if (!SUCCEEDED(GetInstance(pInstruction, ppResult)))
        {
            CComPtr<CBranchTargetInfo> pResult;
            pResult.Attach(new CBranchTargetInfo(pInstruction));
            const GUID* uuid = &__uuidof(CBranchTargetInfo);
            pInstruction->SetDataItem(uuid, uuid, pResult);
            *ppResult = pResult.Detach();
        }

        return S_OK;
    }

    HRESULT CBranchTargetInfo::SetBranchTarget(_In_ CInstruction* pBranch, _In_ CInstruction* pTarget, _In_opt_ CInstruction* pOldTarget)
    {
        IfNullRet(pBranch);
        IfNullRet(pTarget);

        HRESULT hr;

        if (pOldTarget != nullptr)
        {
            CComPtr<CBranchTargetInfo> pOldInfo;

            // There may not be an instance of CBranchTargetInfo if we are in the
            // middle of retargetting a branch. That is OK, just ignore it.
            if (SUCCEEDED(CBranchTargetInfo::GetInstance(pOldTarget, &pOldInfo)))
            {
                pOldInfo->Remove(pBranch);
            }
        }

        CComPtr<CBranchTargetInfo> pInfo;
        IfFailRet(CBranchTargetInfo::GetOrCreateInstance(pTarget, &pInfo));
        pInfo.p->m_branches.emplace(pBranch);
        return S_OK;
    }

    HRESULT CBranchTargetInfo::RetargetBranches(_In_ CInstruction* pOriginalInstruction, _In_ CInstruction* pNewInstruction)
    {
        IfNullRet(pOriginalInstruction);
        IfNullRet(pNewInstruction);

        CComPtr<CBranchTargetInfo> pTargetInfo;
        if (SUCCEEDED(CBranchTargetInfo::GetInstance(pOriginalInstruction, &pTargetInfo)))
        {
            return pTargetInfo->Retarget(pNewInstruction);
        }
        return S_OK;
    }

    HRESULT CBranchTargetInfo::Retarget(_In_ CInstruction* pNewInstruction)
    {
        HRESULT hr = S_OK;

        // keep this alive, since disconnecting branch targets will cause
        // this info to be removed from the data container.
        CComPtr<CBranchTargetInfo> _this = this;

        // Remove this CBranchTargetInfo from the data container. If a new one
        // is needed, it will be regenerated when branch targets are set.
        Disconnect();
        for (CInstruction* pInstr : m_branches)
        {
            if (pInstr->GetIsBranchInternal())
            {
                CBranchInstruction* pBranch = (CBranchInstruction*)pInstr;
                // This indicates that the new instruction targets this instruction.
                // If we were to retarget it, we would cause an infinite loop.
                if (pBranch != pNewInstruction)
                {
                    IfFailRet(pBranch->SetBranchTarget(pNewInstruction));
                }
                else
                {
                    // reset the branch target, since the disconnection
                    // above will have removed it.
                    IfFailRet(pBranch->SetBranchTarget(m_pInstruction));
                }
            }
            else if (pInstr->GetIsSwitchInternal())
            {
                CComPtr<ISwitchInstruction> pSwitch;
                IfFailRet(pInstr->QueryInterface(__uuidof(ISwitchInstruction), (void**)&pSwitch));
                IfFalseRet(pSwitch != nullptr, E_UNEXPECTED);
                IfFailRet(pSwitch->ReplaceBranchTarget(m_pInstruction, pNewInstruction));
            }
        }

        return S_OK;
    }

    void CBranchTargetInfo::Remove(_In_ CInstruction* pOldBranch)
    {
        m_branches.erase(pOldBranch);
        if (m_branches.empty())
        {
            Disconnect();
        }
    }

    void CBranchTargetInfo::Disconnect()
    {
        const GUID* uuid = &__uuidof(CBranchTargetInfo);

        // break cycles.
        m_pInstruction->SetDataItem(uuid, uuid, nullptr);
    }
}

