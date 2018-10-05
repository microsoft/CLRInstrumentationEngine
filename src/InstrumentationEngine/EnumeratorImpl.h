// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    template<class ENUM_INTERFACE, class ENUM_ITEM_TYPE>
    class CEnumerator : public ENUM_INTERFACE, public CDataContainer
    {
    private:
        // CONSIDER: rewrite this using std::vector instead of array.
        // I pulled this from my old concord implementation, but since consumers are most likely using std::vector,
        // it seems like that would be a better impl.
        DWORD m_dwLength;
        CAutoVectorPtr<CComPtr<ENUM_ITEM_TYPE>> m_members;
        DWORD m_dwCurrentIndex;

    public:

        CEnumerator() : m_dwLength(0), m_dwCurrentIndex(0)
        {

        }

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CEnumerator);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CEnumerator);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<ENUM_INTERFACE*>(this),
                riid,
                ppvObject
                );
        }

        HRESULT Initialize(
            _In_ DWORD dwLength,
            _In_reads_(dwLength) ENUM_ITEM_TYPE** members
            )
        {
            HRESULT hr = S_OK;

            if (dwLength != 0)
            {
                m_members.Attach(new CComPtr<ENUM_ITEM_TYPE>[dwLength]);
                if (!m_members)
                {
                    return E_OUTOFMEMORY;
                }

                for (DWORD i = 0; i < m_dwLength; i++)
                {
                    m_members[i] = members[i];
                }
            }

            return hr;
        }

        HRESULT Initialize(
            _In_ vector<CComPtr<ENUM_ITEM_TYPE>>& members
            )
        {
            HRESULT hr = S_OK;

            if (members.size() > 0)
            {
                m_members.Attach(new CComPtr<ENUM_ITEM_TYPE>[members.size()]);
                if (!m_members)
                {
                    return E_OUTOFMEMORY;
                }
                m_dwLength = ((DWORD)members.size());

                DWORD i = 0;
                for (CComPtr<ENUM_ITEM_TYPE> pItem : members)
                {
                    m_members[i] = pItem;
                    i++;
                }
            }

            return hr;
        }

        virtual HRESULT __stdcall  Next(
            _In_ ULONG celt,
            _Out_writes_(celt) ENUM_ITEM_TYPE** rgelt,
            _Out_ ULONG* pceltFetched
            ) override
        {
            HRESULT hr = S_OK;
            memset(rgelt, 0, celt * sizeof(ENUM_ITEM_TYPE*));

            *pceltFetched = 0;
            DWORD dwOrigCurrIndex = m_dwCurrentIndex;

            for (DWORD i = 0; (i < celt) && (i + m_dwCurrentIndex < m_dwLength); i++)
            {
                rgelt[i] = m_members[i + m_dwCurrentIndex];
                rgelt[i]->AddRef();

                m_dwCurrentIndex++;
            }

            *pceltFetched = m_dwCurrentIndex - dwOrigCurrIndex;

            if (celt == *pceltFetched)
            {
                return S_OK;
            }

            return S_FALSE;
        }

        virtual HRESULT __stdcall  Skip(
            _In_ ULONG celt
            ) override
        {
            m_dwCurrentIndex += celt;

            if (m_dwCurrentIndex >= m_dwLength)
            {
                m_dwCurrentIndex = m_dwLength;
                return S_FALSE;
            }

            return S_OK;
        }

        virtual HRESULT __stdcall  Reset(void) override
        {
            m_dwCurrentIndex = 0;

            return S_OK;
        }

        virtual HRESULT __stdcall  Clone(
            _Out_ ENUM_INTERFACE** ppenum
            ) override
        {
            HRESULT hr = S_OK;
            *ppenum = NULL;

            CComPtr<CEnumerator> pEnumerator;
            pEnumerator.Attach(new CEnumerator);
            if (!pEnumerator)
            {
                return E_OUTOFMEMORY;
            }

            IfFailRet(pEnumerator->Initialize(m_dwLength, (ENUM_ITEM_TYPE**)(m_members.m_p)));

            *ppenum = pEnumerator.Detach();

            return hr;
        }

        virtual HRESULT __stdcall  GetCount(_Out_ DWORD* pLength) override
        {
            HRESULT hr = S_OK;
            IfNullRetPointer(pLength);

            *pLength = m_dwLength;
            return S_OK;
        }

    };
}

