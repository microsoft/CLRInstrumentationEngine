// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "MethodInfo.h"

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class CProfilerManager;

    class CCorProfilerFunctionInfoWrapper :
        public ICorProfilerFunctionControl,
        public CModuleRefCount
    {
    private:

        // Non-addref'd Back pointer the profiler manager and method info
        CProfilerManager* m_pProfilerManager;

        CMethodInfo* m_pMethodInfo;

    public:
        CCorProfilerFunctionInfoWrapper(
            _In_ CProfilerManager* pProfilerManager,
            _In_ CMethodInfo* pMethodInfo
            );

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CProfilerManager);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CProfilerManager);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<ICorProfilerFunctionControl*>(this),
                riid,
                ppvObject
                );
        }

    // ICorProfilerFunctionControl
    public:
        virtual HRESULT STDMETHODCALLTYPE SetCodegenFlags(
            _In_ DWORD flags) override;

        virtual HRESULT STDMETHODCALLTYPE SetILFunctionBody(
            _In_ ULONG cbNewILMethodHeader,
            _In_reads_(cbNewILMethodHeader) LPCBYTE pbNewILMethodHeader) override;

        virtual HRESULT STDMETHODCALLTYPE SetILInstrumentedCodeMap(
            _In_ ULONG cILMapEntries,
            _In_reads_(cILMapEntries) COR_IL_MAP rgILMapEntries[  ]) override;
    };
}