// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "pch.h"

PALAPI
VOID
PAL_InitializeCriticalSection(PCRITICAL_SECTION pCriticalSection)
{
    PAL_InitializeCriticalSectionEx(pCriticalSection, 0, 0); // TODO: Add spin count support
}

PALAPI
BOOL
PAL_InitializeCriticalSectionEx(PCRITICAL_SECTION pCriticalSection, DWORD dwSpinCount, DWORD Flags)
{
    pCriticalSection->DebugInfo = nullptr;
    pCriticalSection->LockCount = 0;
    pCriticalSection->RecursionCount = 0;
    pCriticalSection->SpinCount = dwSpinCount;
    pCriticalSection->OwningThread = NULL;
    pCriticalSection->Mutex = NULL;

    pthread_mutex_t* pMutex = new pthread_mutex_t();
    int ret = pthread_mutex_init(pMutex, nullptr);
    VSASSERT(ret == 0, "CS: pthread_mutex_init failed");

    if (ret != 0)
    {
        delete pMutex;
        return FALSE;
    }

    pCriticalSection->Mutex = pMutex;

    return TRUE;
}

PALAPI
VOID
PAL_EnterCriticalSection(PCRITICAL_SECTION pCriticalSection)
{
    DWORD threadId = GetCurrentThreadId();

    // check if the current thread already owns the critical section
    if (pCriticalSection->OwningThread == threadId)
    {
        pCriticalSection->RecursionCount += 1;
        return;
    }

    int ret = pthread_mutex_lock(pCriticalSection->Mutex);
    VSASSERT(ret == 0, "CS: pthread_mutex_lock failed");

    pCriticalSection->OwningThread = threadId;
    pCriticalSection->RecursionCount = 1;
}

PALAPI
BOOL
PAL_TryEnterCriticalSection(PCRITICAL_SECTION pCriticalSection)
{
    DWORD threadId = GetCurrentThreadId();

    // check if the current thread already owns the critical section
    if (pCriticalSection->OwningThread == threadId)
    {
        pCriticalSection->RecursionCount += 1;
        return TRUE;
    }

    int ret = pthread_mutex_trylock(pCriticalSection->Mutex);

    if (ret == 0)
    {
        pCriticalSection->OwningThread = threadId;
        pCriticalSection->RecursionCount = 1;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

PALAPI
VOID
PAL_LeaveCriticalSection(PCRITICAL_SECTION pCriticalSection)
{
#if DEBUG
    DWORD threadId = GetCurrentThreadId();
    VSASSERT(threadId == pCriticalSection->OwningThread, "CS: LeaveCriticalSection invalid thread id");
    VSASSERT(pCriticalSection->RecursionCount > 0, "CS: LeaveCriticalSection called too many times");
#endif

    if (--pCriticalSection->RecursionCount > 0)
        return;

    pCriticalSection->OwningThread = 0;

    int ret = pthread_mutex_unlock(pCriticalSection->Mutex);
    VSASSERT(ret == 0, "CS: pthread_mutex_unlock failed");
}

PALAPI
VOID
PAL_DeleteCriticalSection(PCRITICAL_SECTION pCriticalSection)
{
    VSASSERT(pCriticalSection->OwningThread == 0, "CS: CS still owned by a thread");

    int ret = pthread_mutex_destroy(pCriticalSection->Mutex);
    VSASSERT(ret == 0, "CS: pthread_mutex_destroy failed");

    delete pCriticalSection->Mutex;
    pCriticalSection->Mutex = nullptr;
}


