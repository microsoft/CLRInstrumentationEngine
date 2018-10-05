//
// Copyright (c) Microsoft. All rights reserved.
// 
//
//

//
// ===========================================================================
// File: memorystream.cpp
// 
// ===========================================================================
/*++

Abstract:

    in memory stream


 

Revision History:

--*/

#include "pch.h"
#include <algorithm>
#include "objidl.h"

using namespace std;

class MemoryStream : public IStream
{
    LONG m_cRef; // QI refcount
    ULONG m_nPos; // the current position in the stream
    ULONG m_nSize; // the current size of the stream
    ULONG m_nData; // the size of the allocated data storage, can be < m_nSize
    BYTE* m_pData; // the data storage

private:
    HRESULT Ensure(ULONG nNewData)
    {
        if (nNewData > m_nData)
        {
            // apply some heurestic for growing
            ULONG n = m_nData;

            // grow 2x for smaller sizes, 1.25x for bigger sizes
            n = min(2 * n, n + n / 4 + 0x100000);

            // don't allocate tiny chunks
            n = max(n, static_cast<ULONG>(0x100));

            // compare with the hard limit
            nNewData = max(n, nNewData);
        }
        else
        if (nNewData > m_nData / 4)
        {
            // shrinking but it is not worth it
            return S_OK;
        }

        BYTE * pNewData = (BYTE*)GlobalReAlloc(m_pData, nNewData, 0);
        if (pNewData == NULL && nNewData != 0)
            return E_OUTOFMEMORY;

        m_nData = nNewData;
        m_pData = pNewData;
        return S_OK;
    }

public:
    MemoryStream()
    {
        m_cRef = 1;
        m_nPos = 0;
        m_nSize = 0;
        m_nData = 0;
        m_pData = NULL;
    }

    // NB: assumes the HGLOBAL passed in is a handle to fixed (GMEM_FIXED) memory.
    // This class does not support movable (GMEM_MOVABLE) memory.
    MemoryStream(HGLOBAL existingBuffer, SIZE_T sizeOfBuffer)
    {
        m_cRef = 1;
        m_nPos = 0;
        m_nSize = sizeOfBuffer;
        m_nData = sizeOfBuffer;
        m_pData = (BYTE*)existingBuffer;
    }

#ifdef __GNUC__
    virtual
#endif
    ~MemoryStream()
    {
        GlobalFree(m_pData);
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid,
        void **ppvObject)
    {
        if (riid == IID_IStream ||
            riid == IID_ISequentialStream ||
            riid == IID_IUnknown)
        {
            InterlockedIncrement(&m_cRef);
            *ppvObject = this;
            return S_OK;
        }
        else
        {
            *ppvObject = NULL;
            return E_NOINTERFACE;
        }
    }

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        LONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0)
            delete this;
        return cRef;
    }

    HRESULT STDMETHODCALLTYPE Read(
        void *pv,
        ULONG cb,
        ULONG *pcbRead)
    {
        ULONG nData;
        ULONG nNewPos = m_nPos + cb;

        // check for overflow
        if (nNewPos < cb)
            return STG_E_INVALIDFUNCTION;

        // compare with the actual size
        nNewPos = min(nNewPos, m_nSize);

        // compare with the data available
        nData = min(nNewPos, m_nData);

        // copy the data over
        if (nData > m_nPos)
            memcpy(pv, m_pData + m_nPos, nData - m_nPos);

        // fill the rest with zeros
        if (nNewPos > nData)
            memset((BYTE*)pv + (nData - m_nPos), 0, nNewPos - nData);

        cb = nNewPos - m_nPos;
        m_nPos = nNewPos;

        if (pcbRead)
            *pcbRead = cb;

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Write(
        const void *pv,
        ULONG cb,
        ULONG *pcbWritten)
    {
        ULONG nNewPos = m_nPos + cb;

        // check for overflow
        if (nNewPos < cb)
            return STG_E_INVALIDFUNCTION;

        // ensure the space
        if (nNewPos > m_nData)
        {
            HRESULT hr = Ensure(nNewPos);
            if (FAILED(hr)) return hr;
        }

        // copy the data over
        memcpy(m_pData + m_nPos, pv, cb);

        m_nPos = nNewPos;
        if (m_nPos > m_nSize)
            m_nSize = m_nPos;

        if (pcbWritten)
            *pcbWritten = cb;

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Seek(
        LARGE_INTEGER dlibMove,
        DWORD dwOrigin,
        ULARGE_INTEGER *plibNewPosition)
    {
        ULONG           lStartPos;
        LONGLONG        lNewPos;

        switch (dwOrigin)
        {
        case STREAM_SEEK_SET:
            lStartPos = 0;
            break;
        case STREAM_SEEK_CUR:
            lStartPos = m_nPos;
            break;
        case STREAM_SEEK_END:
            lStartPos = m_nSize;
            break;
        default:
            return STG_E_INVALIDFUNCTION;
        }

        lNewPos = lStartPos + dlibMove.QuadPart;

        // it is an error to seek before the beginning of the stream
        if (lNewPos < 0)
            return STG_E_INVALIDFUNCTION;

        // It is not, however, an error to seek past the end of the stream
        if (lNewPos > m_nSize)
        {
            ULARGE_INTEGER NewSize;
            NewSize.QuadPart = lNewPos;

            HRESULT hr = SetSize(NewSize);
            if (FAILED(hr)) return hr;
        }

        m_nPos = (ULONG)lNewPos;

        if (plibNewPosition != NULL)
            plibNewPosition->QuadPart = m_nPos;

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetSize(
        ULARGE_INTEGER libNewSize)
    {
        if (libNewSize.HighPart != 0)
            return STG_E_INVALIDFUNCTION;

        m_nSize = libNewSize.LowPart;

        // free the space if we are shrinking
        if (m_nSize < m_nData)
            Ensure(m_nSize);

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE CopyTo(
        IStream *pstm,
        ULARGE_INTEGER cb,
        ULARGE_INTEGER *pcbRead,
        ULARGE_INTEGER *pcbWritten)
    {
        _ASSERTE(false);
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Commit(
        DWORD grfCommitFlags)
    {
        _ASSERTE(false);
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Revert()
    {
        _ASSERTE(false);
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE LockRegion(
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD dwLockType)
    {
        _ASSERTE(false);
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE UnlockRegion(
        ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb,
        DWORD dwLockType)
    {
        _ASSERTE(false);
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Stat(
        STATSTG *pstatstg,
        DWORD grfStatFlag)
    {
        memset(pstatstg, 0, sizeof(STATSTG));
        pstatstg->cbSize.QuadPart = m_nSize;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Clone(
        IStream **ppstm)
    {
        _ASSERTE(false);
        return E_NOTIMPL;
    }
};

STDAPI CreateStreamOnHGlobalWithSize(HGLOBAL hGlobal, SIZE_T sizeOfBuffer, BOOL fDeleteOnRelease, IStream** ppstm)
{
    _ASSERTE(fDeleteOnRelease == TRUE || hGlobal == NULL);

    MemoryStream* pStream;

    if (hGlobal != NULL)
    {
        pStream = new MemoryStream(hGlobal, sizeOfBuffer);
    }
    else
    {
        pStream = new MemoryStream;
    }

    if (pStream == NULL) return E_OUTOFMEMORY;

    *ppstm = pStream;
    return S_OK;
}
