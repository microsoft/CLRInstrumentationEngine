// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once
#include "stdafx.h"

class CInstrumentationMethod : public IInstrumentationMethod
{
private:
    std::atomic<int> refCount;

public:
    CInstrumentationMethod() 
    {
    }

    virtual ~CInstrumentationMethod()
    {

    }

    virtual HRESULT STDMETHODCALLTYPE Initialize(IProfilerManager* pProfilerManager)
    {
        CComPtr<ICorProfilerInfo> pCorProfilerInfo;
        pProfilerManager->GetCorProfilerInfo((IUnknown**)&pCorProfilerInfo);
        pCorProfilerInfo->SetEventMask(COR_PRF_MONITOR_EXCEPTIONS | COR_PRF_ENABLE_STACK_SNAPSHOT);
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE OnAppDomainCreated(IAppDomainInfo *pAppDomainInfo) { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE OnAppDomainShutdown(IAppDomainInfo *pAppDomainInfo) { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE OnAssemblyLoaded(IAssemblyInfo* pAssemblyInfo) { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE OnAssemblyUnloaded(IAssemblyInfo* pAssemblyInfo) { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE OnModuleLoaded(IModuleInfo* pModuleInfo)
    {
        HRESULT hr = S_OK;

        char16_t* bstrModuleName;
        IfFailRet(pModuleInfo->GetModuleName(&bstrModuleName));

        char16_t* bstrModulePath;
        IfFailRet(pModuleInfo->GetFullPath(&bstrModulePath));

        /*std::wstring path(bstrModulePath, SysStringLen(bstrModulePath));
        wstring pdbFile = path.substr(0, path.find_last_of(L'.')) + L".pdb";*/

        /*FILE *file;
        _wfopen_s(&file, pdbFile.c_str(), L"r");

        if (file == NULL)
        {
            return S_OK;
        }
        else
        {
            fclose(file);
        }*/

        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE OnModuleUnloaded(IModuleInfo* pModuleInfo) { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE OnShutdown() { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE ShouldInstrumentMethod(IMethodInfo* pMethodInfo, BOOL isRejit, BOOL* pbInstrument) { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE BeforeInstrumentMethod(IMethodInfo* pMethodInfo, BOOL isRejit) { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE InstrumentMethod(IMethodInfo* pMethodInfo, BOOL isRejit) { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE OnInstrumentationComplete(IMethodInfo* pMethodInfo, BOOL isRejit) { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE AllowInlineSite(IMethodInfo* pMethodInfoInlinee, IMethodInfo* pMethodInfoCaller, BOOL* pbAllowInline) { return S_OK; }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override
    {
        if (ppvObject == NULL)
            return E_POINTER;
        *ppvObject = NULL;

        if (riid == __uuidof(IInstrumentationMethod))
        {
            *ppvObject = this;
            this->AddRef();
            return S_OK;
        }

        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef(void) override
    {
        return std::atomic_fetch_add(&this->refCount, 1) + 1;
    }

    ULONG STDMETHODCALLTYPE Release(void) override
    {
        int count = std::atomic_fetch_sub(&this->refCount, 1) - 1;

        if (count <= 0)
        {
            delete this;
        }

        return count;
    }
};