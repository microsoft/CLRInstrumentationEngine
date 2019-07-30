// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy can be used
#include <atomic>
#include <atlsync.h>
#include <memory>
#pragma warning(pop)

#include "CriticalSectionHolder.h"

namespace CommonLib
{
    template<typename T>
    class CSingleton
    {
    private:
        std::atomic_bool m_isCreated;
        CCriticalSection m_cs;
        std::unique_ptr<T> m_pValue;

    public:
        T* const Get()
        {
            if (!m_isCreated.load(std::memory_order::memory_order_acquire))
            {
                CCriticalSectionHolder holder(&m_cs);
                if (!m_isCreated.load(std::memory_order::memory_order_relaxed))
                {
                    m_pValue = std::make_unique<T>();
                    m_isCreated.store(true, std::memory_order::memory_order_release);
                }
            }
            return m_pValue.get();
        }
    };
}