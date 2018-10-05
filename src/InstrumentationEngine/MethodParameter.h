// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "DataContainer.h"
#include "InstrumentationEngine.h"

namespace MicrosoftInstrumentationEngine
{
    class __declspec(uuid("26255678-9F51-433F-89B1-51B978EB4C2B"))
    CMethodParameter : public IMethodParameter, public CDataContainer
    {
        private:
            CComPtr<IType> m_pType;
        public:
            DEFINE_DELEGATED_REFCOUNT_ADDREF(CMethodParameter);
            DEFINE_DELEGATED_REFCOUNT_RELEASE(CMethodParameter);
            STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
            {
                return ImplQueryInterface(
                    static_cast<IMethodParameter*>(this),
                    static_cast<IDataContainer*>(static_cast<CDataContainer*>(this)),
                    riid,
                    ppvObject
                    );
            }

        public:
            CMethodParameter(_In_ IType* parameterType);
        public:
            virtual HRESULT __stdcall GetType(_Out_ IType** ppType) override;
    };
}