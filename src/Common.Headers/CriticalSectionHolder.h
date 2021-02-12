// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CommonLib
{
    class CCriticalSectionHolder
    {
        LPCRITICAL_SECTION m_pLock;
        bool m_bHoldsLock;

    public:
        explicit CCriticalSectionHolder(_In_ LPCRITICAL_SECTION pLock)
        {
            m_pLock = pLock;
            ::EnterCriticalSection(pLock);

            m_bHoldsLock = true;
        }

        ~CCriticalSectionHolder()
        {
            ReleaseLock();
        }

        void ReleaseLock()
        {
            if (m_bHoldsLock)
            {
                ::LeaveCriticalSection(m_pLock);
                m_bHoldsLock = false;
            }
        }

        void RetakeLock()
        {
            //VSASSERT(!m_bHoldsLock, L"Already holds lock");
            ::EnterCriticalSection(m_pLock);
            m_bHoldsLock = true;
        }
    };

    /*++

    Routine Description:

     This function checks if a critical section is owned or not.

    --*/
    inline bool IsCriticalSectionOwned(
        _In_ const CRITICAL_SECTION* lpCriticalSection
    )
    {
        DWORD threadId = GetCurrentThreadId();

        //
        // this is actually thread safe
        //
        return (DWORD_PTR)lpCriticalSection->OwningThread == threadId;
    }
}