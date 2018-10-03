// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

// COWAIT_DISPATCH_WINDOW_MESSAGES is explicitly left out of the valid flags.

static const DWORD VALID_COWAIT_FLAGS =
    COWAIT_WAITALL | COWAIT_ALERTABLE | COWAIT_INPUTAVAILABLE | COWAIT_DISPATCH_CALLS;

WINOLEAPI CoWaitForMultipleHandles (DWORD dwFlags,
                                    DWORD dwTimeout,
                                    ULONG cHandles,
                                    LPHANDLE pHandles,
                                    LPDWORD  lpdwindex)
{
    HRESULT         hr = S_OK;
    DWORD          dwSignaled = 0;

    if ((pHandles == NULL) || (lpdwindex == NULL))
    {
        if (lpdwindex) *lpdwindex = 0;
        return E_INVALIDARG;
    }

    if ((dwFlags & ~VALID_COWAIT_FLAGS) != 0)
    {
        *lpdwindex = 0;
        return E_INVALIDARG;
    }

    // If nothing to do, return
    if (cHandles == 0)
    {
        *lpdwindex = 0;
        return RPC_E_NO_SYNC;
    }

    // Use the default implementation
    dwSignaled = WaitForMultipleObjectsEx(cHandles,
                                          pHandles,
                                          dwFlags & COWAIT_WAITALL,
                                          dwTimeout,
                                          dwFlags & COWAIT_ALERTABLE);

    // Fix up the error code if the wait timed out.

    if (dwSignaled == WAIT_TIMEOUT)
    {
        hr = RPC_S_CALLPENDING;
    }
    else if (((LONG) dwSignaled < WAIT_OBJECT_0) ||
             (dwSignaled >= WAIT_OBJECT_0 + cHandles))
    {
        hr = GetLastError();
    }
    else
    {
        dwSignaled -= WAIT_OBJECT_0;
    }

    *lpdwindex = dwSignaled;
    return hr;
}

