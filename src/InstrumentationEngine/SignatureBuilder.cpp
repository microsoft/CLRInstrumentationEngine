// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "SignatureBuilder.h"


namespace MicrosoftInstrumentationEngine
{
    CSignatureBuilder::CSignatureBuilder()
        : m_memory(nullptr), m_used(0), m_capacity(0)
    {
        DEFINE_REFCOUNT_NAME(CSignatureBuilder);
    }

    CSignatureBuilder::~CSignatureBuilder()
    {
        if (m_memory != nullptr)
        {
            delete[] m_memory;
        }
    }

    HRESULT CSignatureBuilder::Add(_In_ ULONG x)
    {
        // CorSigCompressData returns a max of 4 bytes
        HRESULT hr = S_OK;
        IfFailRet(EnsureCapacity(m_used + sizeof(ULONG)));
        m_used += CorSigCompressData(x, m_memory + m_used);
        return hr;
    }

    HRESULT CSignatureBuilder::AddSignedInt(_In_ LONG y)
    {
        HRESULT hr = S_OK;
        // CorSigCompressSignedInt returns a max of 4 bytes
        IfFailRet(EnsureCapacity(m_used + sizeof(LONG)));
        m_used += CorSigCompressSignedInt(y, m_memory + m_used);
        return hr;
    }

    HRESULT CSignatureBuilder::AddToken(_In_ mdToken t)
    {
        HRESULT hr = S_OK;
        // CorSigCompressToken returns a max of 4 bytes
        IfFailRet(EnsureCapacity(m_used + sizeof(mdToken)));
        m_used += CorSigCompressToken(t, m_memory + m_used);
        return hr;
    }

    HRESULT CSignatureBuilder::AddElementType(_In_ CorElementType x)
    {
        HRESULT hr = S_OK;
        // CorElementType is a 1 byte element
        IfFailRet(EnsureCapacity(m_used + 1));
        m_memory[m_used++] = x;
        return hr;
    }

    HRESULT CSignatureBuilder::AddData(_In_ const BYTE* memory, _In_ DWORD cbSize)
    {
        HRESULT hr = S_OK;
        if (cbSize)
        {
            IfFailRet(EnsureCapacity(m_used + cbSize));
            IfFailRetErrno(memcpy_s(m_memory + m_used, cbSize, memory, cbSize));
            m_used += cbSize;
        }
        return hr;
    }

    HRESULT CSignatureBuilder::AddSignature(_In_ ISignatureBuilder* pSignature)
    {
        IfNullRetPointer(pSignature);
        HRESULT hr = S_OK;

        const BYTE* signature;
        DWORD size;
        IfFailRet(pSignature->GetCorSignaturePtr(&signature));
        IfFailRet(pSignature->GetSize(&size));
        IfFailRet(AddData(signature, size));

        return hr;
    }

    HRESULT CSignatureBuilder::Clear()
    {
        if (m_memory != nullptr)
        {
            delete[] m_memory;
        }

        m_memory = nullptr;
        m_used = 0;
        m_capacity = 0;

        return S_OK;
    }

    HRESULT CSignatureBuilder::GetCorSignature(_In_ DWORD cbBuffer, _Inout_opt_ BYTE* pCorSignature, _Out_opt_ DWORD* pcbSignature)
    {
        HRESULT hr = S_OK;
        if (pCorSignature == nullptr)
        {
            if (pcbSignature != nullptr)
            {
                *pcbSignature = m_used;
            }
            return S_OK;
        }
        if (cbBuffer < m_used)
        {
            return E_INVALIDARG;
        }
        if (m_memory != nullptr)
        {
            IfFailRetErrno(memcpy_s(pCorSignature, cbBuffer, m_memory, m_used));
        }

        if (pcbSignature != nullptr)
        {
            *pcbSignature = m_used;
        }
        return S_OK;
    }

    HRESULT CSignatureBuilder::GetCorSignaturePtr(_Out_ const BYTE** ppCorSignature)
    {
        IfNullRetPointer(ppCorSignature);
        *ppCorSignature = this->m_memory;

        if (m_memory == nullptr)
        {
            return S_FALSE;
        }
        return S_OK;
    }

    HRESULT CSignatureBuilder::GetSize(_Out_ DWORD* cbSize)
    {
        IfNullRetPointer(cbSize);
        *cbSize = m_used;
        return S_OK;
    }

    HRESULT CSignatureBuilder::EnsureCapacity(_In_ DWORD needed)
    {
        HRESULT hr = S_OK;
        if (m_capacity < needed)
        {
            DWORD newCapacity = m_capacity * 2 + needed;
            BYTE* newMemory = new BYTE[newCapacity];
            if (newMemory == nullptr)
            {
                return E_OUTOFMEMORY;
            }

            if (m_memory != nullptr)
            {
                IfFailRetErrno(memcpy_s(newMemory, newCapacity, m_memory, newCapacity < m_capacity ? newCapacity : m_capacity));
                delete[] m_memory;
            }
            m_memory = newMemory;
            m_capacity = newCapacity;
        }
        return hr;
    }
}

