// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "DataContainer.h"

MicrosoftInstrumentationEngine::CDataContainer::CDataContainer()
{
    DEFINE_REFCOUNT_NAME(CDataContainer);

    InitializeCriticalSection(&m_cs);
}

MicrosoftInstrumentationEngine::CDataContainer::~CDataContainer()
{
    CCriticalSectionHolder lock(&m_cs);

    // NOTE: Neither shared_ptr or CAutoPtr will work as template parameters to CAtlMap. So,
    // manually deleting the sub maps
    POSITION pos = m_dataContainerMap.GetStartPosition();
    while (pos != nullptr)
    {
        CAtlMap<GUID, CComPtr<IUnknown>>* pMap = m_dataContainerMap.GetNextValue(pos);
        delete pMap;
    }

    lock.ReleaseLock();

    DeleteCriticalSection(&m_cs);
}

HRESULT MicrosoftInstrumentationEngine::CDataContainer::SetDataItem(
    _In_ const GUID* pComponentId,
    _In_ const GUID* pObjectGuid,
    _In_ IUnknown* pDataItem
    )
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder lock(&m_cs);

    CAtlMap<GUID, CComPtr<IUnknown>>* pMap;

    if (m_dataContainerMap.Lookup(*pComponentId, pMap) == false)
    {
        pMap = new CAtlMap<GUID, CComPtr<IUnknown>>;
        if (!pMap)
        {
            return E_OUTOFMEMORY;
        }

        m_dataContainerMap.SetAt(*pComponentId, pMap);
    }

    pMap->SetAt(*pObjectGuid, CComPtr<IUnknown>(pDataItem));

    return hr;
}


HRESULT MicrosoftInstrumentationEngine::CDataContainer::GetDataItem(
    _In_ const GUID* pComponentId,
    _In_ const GUID* pObjectGuid,
    _Deref_out_ IUnknown** ppDataItem
    )
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder lock(&m_cs);

    CAtlMap<GUID, CComPtr<IUnknown>>* pMap;

    if (m_dataContainerMap.Lookup(*pComponentId, pMap) == true)
    {
        CComPtr<IUnknown> pDataItem;
        if (pMap != nullptr && pMap->Lookup(*pObjectGuid, pDataItem) == true)
        {
            if (pDataItem != nullptr)
            {
                *ppDataItem = pDataItem.Detach();
                return S_OK;
            }
        }
    }

    return E_FAIL;
}
