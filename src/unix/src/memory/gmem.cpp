//
// Copyright (c) Microsoft. All rights reserved.
//
//

/*++



Module Name:

gmem.c

Abstract:

Implementation of global memory management functions.

Revision History:



--*/

#include "pch.h"
#include "heap.h"

/*++
Function:
GlobalAlloc

See MSDN doc.
--*/
PALAPI
HGLOBAL
GlobalAlloc(
    UINT uFlags,
    SIZE_T uBytes)
{
    // guard against unsupported flags
    UINT supportedFlags = GMEM_FIXED | GMEM_ZEROINT;
    if ((uFlags | supportedFlags) != supportedFlags)
    {
        ASSERT("Invalid parameter uFlags=%#x\n", uFlags);
        SetLastError(ERROR_INVALID_PARAMETER);
        LOGEXIT("GlobalAlloc returning NULL\n");
        return NULL;
    }

    // translate known flags to their HeapAlloc equivalents
    DWORD heapAllocFlags = 0;
    if ((uFlags & GMEM_ZEROINT) != 0)
        heapAllocFlags |= HEAP_ZERO_MEMORY;

    return HeapAlloc(GetProcessHeap(), heapAllocFlags, uBytes);
}

/*++
Function:
GlobalReAlloc

See MSDN doc.
--*/
PALAPI
HGLOBAL
GlobalReAlloc(
    HLOCAL hMem,
    SIZE_T uBytes,
    UINT uFlags)
{
    // guard against unsupported flags
    UINT supportedFlags = GMEM_FIXED | GMEM_ZEROINT;
    if ((uFlags | supportedFlags) != supportedFlags)
    {
        ASSERT("Invalid parameter uFlags=%#x\n", uFlags);
        SetLastError(ERROR_INVALID_PARAMETER);
        LOGEXIT("GlobalAlloc returning NULL\n");
        return NULL;
    }

    // translate known flags to their HeapReAlloc equivalents
    DWORD heapReAllocFlags = 0;
    if ((uFlags & GMEM_ZEROINT) != 0)
        heapReAllocFlags |= HEAP_ZERO_MEMORY;

    return HeapReAlloc(GetProcessHeap(), heapReAllocFlags, hMem, uBytes);
}

/*++
Function:
GlobalFree

See MSDN doc.
--*/
PALAPI
HGLOBAL
GlobalFree(
    HGLOBAL hMem)
{
    if (HeapFree(GetProcessHeap(), 0, hMem))
    {
        return NULL;
    }
    else
    {
        return hMem;
    }
}

/*++
Function:
GlobalLock

See MSDN doc.
--*/
PALAPI
LPVOID
GlobalLock(
    HGLOBAL hMem)
{
    // because we don't support GMEM_MOVEABLE, this function does nothing.
    return hMem;
}

/*++
Function:
GlobalUnlock

See MSDN doc.
--*/
PALAPI
BOOL
GlobalUnlock(
    HGLOBAL hMem)
{
    // because we don't support GMEM_MOVEABLE, this function always returns true.
    return true;
}