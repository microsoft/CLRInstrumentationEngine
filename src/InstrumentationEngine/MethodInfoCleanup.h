// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once
#include "MethodInfo.h"

namespace MicrosoftInstrumentationEngine
{
    // Simple class to call cleanup on the method info in the destructor.
    class CCleanupMethodInfo
    {
    private:
        CComPtr<CMethodInfo> m_pMethodInfo;
    public:
        CCleanupMethodInfo()
        {

        }

        CCleanupMethodInfo(_In_ CMethodInfo* pMethodInfo)
        {
            m_pMethodInfo = pMethodInfo;
        }

        ~CCleanupMethodInfo()
        {
            if (m_pMethodInfo != nullptr)
            {
                m_pMethodInfo->Cleanup();
                m_pMethodInfo.Release();
            }
        }

        HRESULT SetMethodInfo(CMethodInfo* pMethodInfo)
        {
            m_pMethodInfo = pMethodInfo;
            return S_OK;
        }

        void Detach()
        {
            m_pMethodInfo = nullptr;
        }
    };
}

