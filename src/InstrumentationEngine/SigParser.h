// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

//------------------------------------------------------------------------
// Encapsulates how compressed integers and typeref tokens are encoded into
// a bytestream.
//------------------------------------------------------------------------
namespace MicrosoftInstrumentationEngine
{
    class SigParser
    {
    private:
        PCCOR_SIGNATURE m_ptr;
        DWORD           m_dwLen;

        FORCEINLINE void SkipBytes(_In_ ULONG cb)
        {
            if (cb > m_dwLen)
                CLogging::LogError(_T("cb <= m_dwLen"));
            m_ptr += cb;
            m_dwLen -= cb;
        }

    public:
        FORCEINLINE SigParser(_In_ PCCOR_SIGNATURE ptr, _In_ DWORD len)
        {
            m_ptr = ptr;
            m_dwLen = len;

            if (m_ptr == nullptr)
            {
                ATLASSERT(!L"m_ptr == nullptr");
            }
        }

        //------------------------------------------------------------------------
        // Remove one compressed integer (using CorSigUncompressData) from
        // the head of the stream and return it.
        //------------------------------------------------------------------------
        __checkReturn
            FORCEINLINE HRESULT GetData(_Out_ ULONG* data)
        {
            ULONG sizeOfData = 0;
            ULONG tempData = 0;

            if (data == nullptr)
                data = &tempData;

            HRESULT hr = CorSigUncompressData(m_ptr, m_dwLen, data, &sizeOfData);

            if (SUCCEEDED(hr))
            {
                SkipBytes(sizeOfData);
            }

            return hr;
        }


        //-------------------------------------------------------------------------
        // Remove one byte and return it.
        //-------------------------------------------------------------------------
        __checkReturn
            FORCEINLINE HRESULT GetByte(_Out_ BYTE *data)
        {
            if (m_dwLen > 0)
            {
                if (data != nullptr)
                    *data = *m_ptr;

                SkipBytes(1);

                return S_OK;
            }

            if (data != nullptr)
                *data = 0;
            return META_E_BAD_SIGNATURE;
        }

        //-------------------------------------------------------------------------
        // Peek at value of one byte and return it.
        //-------------------------------------------------------------------------
        __checkReturn
            FORCEINLINE HRESULT PeekByte(_Out_ BYTE *data) const
        {
            if (m_dwLen > 0)
            {
                if (data != nullptr)
                    *data = *m_ptr;

                return S_OK;
            }

            if (data != nullptr)
                *data = 0;
            return META_E_BAD_SIGNATURE;
        }

        // Inlined version
        __checkReturn
            FORCEINLINE HRESULT GetElemType(_Out_ CorElementType * etype)
        {
            if (m_dwLen > 0)
            {
                CorElementType typ = (CorElementType)* m_ptr;
                if (etype != nullptr)
                {
                    *etype = typ;
                }
                SkipBytes(1);
                return S_OK;
            }
            return META_E_BAD_SIGNATURE;
        }

        // Note: Calling convention is always one byte, not four bytes
        __checkReturn
            HRESULT PeekCallingConvInfo(_Out_ ULONG *data)
        {
            ULONG tmpData;

            if (data == nullptr)
                data = &tmpData;

            HRESULT hr = CorSigUncompressCallingConv(m_ptr, m_dwLen, data);

            return hr;
        }

        // Note: Calling convention is always one byte, not four bytes
        __checkReturn
            HRESULT GetCallingConvInfo(_Out_ ULONG * data)
        {
            ULONG tmpData;

            if (data == nullptr)
                data = &tmpData;

            HRESULT hr = CorSigUncompressCallingConv(m_ptr, m_dwLen, data);
            if (SUCCEEDED(hr))
            {
                SkipBytes(1);
            }

            return hr;
        }

        __checkReturn
            HRESULT GetCallingConv(_Out_ ULONG* data)
        {
            ULONG info;
            HRESULT hr = GetCallingConvInfo(&info);

            if (SUCCEEDED(hr) && data != nullptr)
            {
                *data = IMAGE_CEE_CS_CALLCONV_MASK & info;
            }

            return hr;
        }

        //------------------------------------------------------------------------
        // Non-destructive read of compressed integer.
        //------------------------------------------------------------------------
        __checkReturn
            HRESULT PeekData(_Out_ ULONG *data) const
        {
            if (data == nullptr)
                CLogging::LogError(_T("data == nullptr"));

            ULONG sizeOfData = 0;
            return CorSigUncompressData(m_ptr, m_dwLen, data, &sizeOfData);
        }


        //------------------------------------------------------------------------
        // Non-destructive read of element type.
        //------------------------------------------------------------------------
        __checkReturn
            FORCEINLINE HRESULT PeekElemType(_Out_ CorElementType *etype) const
        {
            if (etype == nullptr)
                CLogging::LogError(_T("etype == nullptr"));

            if (m_dwLen > 0)
            {
                CorElementType typ = (CorElementType)* m_ptr;
                *etype = typ;
                return S_OK;
            }

            return META_E_BAD_SIGNATURE;
        }

        //------------------------------------------------------------------------
        // Is this at the Sentinal (the ... in a varargs signature) that marks
        // the begining of varguments that are not decared at the target

        bool AtSentinel() const
        {
            if (m_dwLen > 0)
                return *m_ptr == ELEMENT_TYPE_SENTINEL;
            else
                return false;
        }

        //------------------------------------------------------------------------
        // Removes a compressed metadata token and returns it.
        //------------------------------------------------------------------------
        __checkReturn
            FORCEINLINE
            HRESULT GetToken(_Out_ mdToken * token)
        {
            DWORD dwLen;
            mdToken tempToken;

            if (token == nullptr)
                token = &tempToken;

            HRESULT hr = CorSigUncompressToken(m_ptr, m_dwLen, token, &dwLen);

            if (SUCCEEDED(hr))
            {
                SkipBytes(dwLen);
            }

            return hr;
        }

    };  // class SigParser
}