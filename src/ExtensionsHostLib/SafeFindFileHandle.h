// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "stdafx.h"

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