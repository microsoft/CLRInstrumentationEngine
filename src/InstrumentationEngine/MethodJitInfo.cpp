// Copyright (c) Microsoft Corporation. All rights reserved.
// 
#include "stdafx.h"
#include "InstrumentationEngine.h"
#include "MethodJitInfo.h"
#include "ProfilerManager.h"

namespace MicrosoftInstrumentationEngine
{
    HRESULT CMethodJitInfo::GetFunctionID(_Out_ FunctionID* pFunctionId)
    {
        IfNullRet(pFunctionId);
        *pFunctionId = m_functionId;
        return S_OK;
    }

    HRESULT CMethodJitInfo::GetIsRejit(_Out_ BOOL* pIsRejit)
    {
        IfNullRet(pIsRejit);
        *pIsRejit = m_isRejit;
        return S_OK;
    }

    HRESULT CMethodJitInfo::GetRejitId(_Out_ ReJITID* pRejitId)
    {
        IfNullRet(pRejitId);
        *pRejitId = m_rejitId;
        return S_OK;
    }

    HRESULT CMethodJitInfo::GetIsInstrumented(_Out_ BOOL* pIsInstrumented)
    {
        IfNullRet(pIsInstrumented);

        HRESULT hr;
        IfFailRet(EnsureInitialized());
        *pIsInstrumented = m_isInstrumented;

        return S_OK;
    }

    HRESULT CMethodJitInfo::GetModuleInfo(_Out_ IModuleInfo** ppModuleInfo)
    {
        IfNullRet(ppModuleInfo);
        HRESULT hr;
        IfFailRet(EnsureInitialized());
        CComPtr<IModuleInfo> pModuleInfo(m_pModuleInfo);
        *ppModuleInfo = pModuleInfo.Detach();
        return S_OK;
    }

    HRESULT CMethodJitInfo::GetJitHR(_Out_ HRESULT* pHr)
    {
        IfNullRet(pHr);
        *pHr = m_hresult;
        return S_OK;
    }

    HRESULT CMethodJitInfo::EnsureInitialized()
    {
        if (m_initializeResult != S_OK)
        {
            return m_initializeResult;
        }

        auto initialize = [this]() -> HRESULT
        {
            if (m_pModuleInfo == nullptr)
            {
                HRESULT hr;
                CComPtr<IAppDomainCollection> pAppDomainCollection;
                IfFailRet(m_pProfilerManager->GetAppDomainCollection(&pAppDomainCollection));

                CComPtr<ICorProfilerInfo> pProfilerInfo;
                IfFailRet(m_pProfilerManager->GetCorProfilerInfo((IUnknown**)(&pProfilerInfo)));

                ClassID classId;
                ModuleID moduleId;
                IfFailRet(pProfilerInfo->GetFunctionInfo(m_functionId, &classId, &moduleId, &m_methodToken));

                IfFailRet(pAppDomainCollection->GetModuleInfoById(moduleId, &m_pModuleInfo));

                CModuleInfo* pModuleInfo = static_cast<CModuleInfo*>(m_pModuleInfo.p);

                m_isInstrumented = pModuleInfo->GetIsMethodInstrumented(m_methodToken);
            }

            return S_OK;
        };

        m_initializeResult = initialize();
        return m_initializeResult;
    }
}