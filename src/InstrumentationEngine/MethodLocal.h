// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "DataContainer.h"
#include "InstrumentationEngine.h"

namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("F09FE4FB-AB00-4F6B-AF5F-B11CF8F4E436"))
    CMethodLocal : public IMethodLocal, public CDataContainer
    {
    private:
        CComPtr<IType> m_pType;

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CMethodLocal);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CMethodLocal);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IMethodLocal*>(this),
                static_cast<IDataContainer*>(static_cast<CDataContainer*>(this)),
                riid,
                ppvObject
            );
        }

    public:
        CMethodLocal(_In_ IType* pType);

    // IMethodLocal
    public:
        virtual HRESULT __stdcall GetType(_Out_ IType** ppType) override;
    };
}