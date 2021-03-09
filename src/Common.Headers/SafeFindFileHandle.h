// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifndef PLATFORM_UNIX
#include <Windows.h>
#endif

namespace CommonLib
{
    class SafeFindFileHandle
    {
    public:
        SafeFindFileHandle(_In_ HANDLE fileHandle)
        {
            m_handle = fileHandle;
        }

        operator HANDLE() const
        {
            return m_handle;
        }

        ~SafeFindFileHandle()
        {
            FindClose(m_handle);
        }

    private:
        HANDLE m_handle;
    };
}