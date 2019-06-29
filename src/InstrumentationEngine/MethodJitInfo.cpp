// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "InstrumentationEngine.h"
#include "ModuleInfo.h"
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

    HRESULT CMethodJitInfo::GetILTransformationStatus(_Out_ BOOL* pIsTransformed)
    {
        IfNullRet(pIsTransformed);
        HRESULT hr;
        IfFailRet(EnsureInitialized());
        *pIsTransformed = m_isTransformed;
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

    HRESULT CMethodJitInfo::GetILNativeMapping(_In_ ULONG32 cMap, _Out_writes_(cMap) COR_DEBUG_IL_TO_NATIVE_MAP* pMap, _Out_ ULONG32* pcNeeded)
    {
        HRESULT hr;
        IfNullRet(pcNeeded);

        CComPtr<IUnknown> pProfilerInfo;
        IfFailRet(m_pProfilerManager->GetCorProfilerInfo(&pProfilerInfo));

        // On Core clr, this should take care of tiered JIT
        UINT_PTR nativeAddress;
        if (SUCCEEDED(GetNativeCodeAddress(&nativeAddress)))
        {
            CComPtr<ICorProfilerInfo9> pProfilerInfo9;
            IfFailRet(pProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo9), (void**)&pProfilerInfo9));
            IfFailRet(pProfilerInfo9->GetILToNativeMapping3(nativeAddress, cMap, pcNeeded, pMap));
            return S_OK;
        }

        CComPtr<ICorProfilerInfo4> pProfilerInfo4;
        if (SUCCEEDED(pProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo4), (void**)&pProfilerInfo4)))
        {
            // Note, this will return E_NOTIMPL before framework 4.8, so we don't
            // assert on failure.
            IfFailRet(pProfilerInfo4->GetILToNativeMapping2(m_functionId, m_rejitId, cMap, pcNeeded, pMap));
            return S_OK;
        }

        return E_NOTIMPL;
    }

    HRESULT CMethodJitInfo::GetILInstrumentationMap(_In_ ULONG32 cMap, _Out_writes_(cMap) COR_IL_MAP* pMap, _Out_ ULONG32* pcNeeded)
    {
        HRESULT hr;
        IfNullRet(pcNeeded);

        IfFailRet(EnsureInitialized());

        return m_pModuleInfo->GetILInstrumentationMap(this, cMap, pMap, pcNeeded);
    }

    HRESULT CMethodJitInfo::GetMethodToken(_Out_ mdMethodDef* pMethodDef)
    {
        HRESULT hr;
        IfNullRet(pMethodDef);
        IfFailRet(EnsureInitialized());
        *pMethodDef = m_methodToken;
        return S_OK;
    }

    HRESULT CMethodJitInfo::EnsureInitialized()
    {
        if (m_initializeResult != S_OK)
        {
            return m_initializeResult;
        }

        if (m_pModuleInfo == nullptr)
        {
            // A lambda is used here so that we can use IfFailRet inside this function
            // to cache the initialization result without having to add another function to
            // the class interface.
            auto initialize = [this]() -> HRESULT
            {
                HRESULT hr;
                CComPtr<IAppDomainCollection> pAppDomainCollection;
                IfFailRet(m_pProfilerManager->GetAppDomainCollection(&pAppDomainCollection));

                CComPtr<ICorProfilerInfo> pProfilerInfo;
                IfFailRet(m_pProfilerManager->GetCorProfilerInfo((IUnknown**)(&pProfilerInfo)));

                ClassID classId;
                ModuleID moduleId;
                IfFailRet(pProfilerInfo->GetFunctionInfo(m_functionId, &classId, &moduleId, &m_methodToken));

                CComPtr<IModuleInfo> pModuleInfo;
                IfFailRet(pAppDomainCollection->GetModuleInfoById(moduleId, &pModuleInfo));

                m_pModuleInfo = static_cast<CModuleInfo*>(pModuleInfo.p);

                m_isTransformed = m_pModuleInfo->GetIsMethodInstrumented(m_methodToken);

                return S_OK;
            };

            // cache the result of initializatin so that we don't try again on failure.
            m_initializeResult = initialize();
        }

        return m_initializeResult;
    }

    HRESULT CMethodJitInfo::GetNativeCodeAddress(_Out_ UINT_PTR *pCodeAddress)
    {
        IfNullRet(pCodeAddress);

        if (m_isAddressQueried)
        {
            if (m_codeAddress != 0)
            {
                *pCodeAddress = m_codeAddress;
                return S_OK;
            }
            else
            {
                return E_NOTIMPL;
            }
        }

        m_isAddressQueried = true;
        HRESULT hr;
        CComPtr<ICorProfilerInfo> pProfilerInfo;
        IfFailRet(m_pProfilerManager->GetCorProfilerInfo((IUnknown**)(&pProfilerInfo)));

        CComPtr<ICorProfilerInfo9> pProfilerInfo9;
        if (pProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo9), (void**)&pProfilerInfo9) != S_OK)
        {
            return E_NOTIMPL;
        }

        ULONG32 cAddresses;
        IfFailRet(pProfilerInfo9->GetNativeCodeStartAddresses(m_functionId, m_rejitId, 0, &cAddresses, nullptr));

        unique_ptr<UINT_PTR[]> addresses = make_unique<UINT_PTR[]>(cAddresses);
        IfFailRet(pProfilerInfo9->GetNativeCodeStartAddresses(m_functionId, m_rejitId, cAddresses, &cAddresses, addresses.get()));

        // we are only interested in the last jitted one.
        m_codeAddress = addresses[cAddresses - 1];
        *pCodeAddress = m_codeAddress;
        return S_OK;
    }
}