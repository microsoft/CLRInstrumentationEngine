// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "DataContainer.h"

MicrosoftInstrumentationEngine::CDataContainer::CDataContainer() : m_dataContainerMap(1)
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
    _In_opt_ IUnknown* pDataItem
    )
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder lock(&m_cs);

    CAtlMap<GUID, CComPtr<IUnknown>>* pMap = nullptr;

    if (m_dataContainerMap.Lookup(*pComponentId, pMap) == false)
    {
        // Only create the new map if we aren't setting the
        // data item to null.
        if (pDataItem != nullptr)
        {
            pMap = new CAtlMap<GUID, CComPtr<IUnknown>>(1);
            if (!pMap)
            {
                return E_OUTOFMEMORY;
            }

            m_dataContainerMap.SetAt(*pComponentId, pMap);
        }
    }

    if (pMap != nullptr)
    {
        // if the data item is null, free memory.
        if (pDataItem == nullptr)
        {
            pMap->RemoveKey(*pObjectGuid);

            if (pMap->GetCount() == 0)
            {
                m_dataContainerMap.RemoveKey(*pComponentId);

                // need to manually delete the map.
                delete pMap;
            }
        }
        else
        {
            pMap->SetAt(*pObjectGuid, CComPtr<IUnknown>(pDataItem));
        }
    }

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
