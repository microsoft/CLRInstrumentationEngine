// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class CDataContainer : public IDataContainer, public CModuleRefCount
    {
    private:
        CRITICAL_SECTION m_cs;

        // LOCKING: hold m_cs
        CAtlMap<GUID, CAtlMap<GUID, CComPtr<IUnknown>>*> m_dataContainerMap;

    public:
        CDataContainer();
        ~CDataContainer();

    public:
        virtual HRESULT __stdcall SetDataItem(
            _In_ const GUID* componentId,
            _In_ const GUID* objectGuid,
            _In_ IUnknown* pDataItem
            );

        virtual HRESULT __stdcall GetDataItem(
            _In_ const GUID* componentId,
            _In_ const GUID* objectGuid,
            _Deref_out_ IUnknown** ppDataItem
            );
    };
}