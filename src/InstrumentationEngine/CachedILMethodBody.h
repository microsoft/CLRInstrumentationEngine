// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    // The profiler apis have issues around rejit request revert and generics
    // If a function is modified by the profiler, GetILFunctionBody may return
    // the modified IL (example: rejit before original jit).
    // So, if any consumer requests the il function body, the original il is cached
    // and resused in requests that come later.
    //
    // NOTE: this is unfortunate as this is yet another copy of the il.
    class CCachedILMethodBody
    {
    private:
        mdToken m_methodToken;
        IMAGE_COR_ILMETHOD* m_pMethodHeader;
        ULONG m_cbMethodSize;

    public:
        CCachedILMethodBody(
            _In_ mdToken methodToken,
            _In_ IMAGE_COR_ILMETHOD* pMethodHeader,
            _In_ ULONG cbMethodSize
            )
        {
            m_methodToken = methodToken;
            m_pMethodHeader = pMethodHeader;
            m_cbMethodSize = cbMethodSize;
        }

    public:
        HRESULT GetOriginalMethodBody(
            _Out_ IMAGE_COR_ILMETHOD** ppMethodHeader,
            _Out_ ULONG* pcbMethodSize
            )
        {
            HRESULT hr = S_OK;

            *ppMethodHeader = m_pMethodHeader;
            *pcbMethodSize = m_cbMethodSize;

            return hr;
        }
    };
}