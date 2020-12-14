// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy can be used
#include <atomic>
#include <functional>
#include <atlsync.h>
#pragma warning(pop)

#include "CriticalSectionHolder.h"

namespace CommonLib
{
    class CInitOnce
    {
    private:
        std::atomic_bool m_isCreated;
        CCriticalSection m_cs;
        std::function<HRESULT()> m_func;
        HRESULT m_result;

    public:
        CInitOnce(const std::function<HRESULT()>& func) : m_isCreated(false), m_func(func), m_result(0) { }

    public:
        HRESULT Get()
        {
            if (!m_isCreated.load(std::memory_order_acquire))
            {
                CCriticalSectionHolder holder(&m_cs);
                if (!m_isCreated.load(std::memory_order_relaxed))
                {
                    m_result = m_func();
                    m_isCreated.store(true, std::memory_order_release);
                }
            }
            return m_result;
        }

        bool IsSuccessful() const
        {
            return m_isCreated.load(std::memory_order_relaxed) && SUCCEEDED(m_result);
        }

        HRESULT Reset()
        {
            if (m_isCreated.load(std::memory_order_acquire))
            {
                CCriticalSectionHolder holder(&m_cs);
                if (m_isCreated.load(std::memory_order_relaxed))
                {
                    m_result = S_OK;
                    m_isCreated.store(false, std::memory_order_release);
                }
            }

            return m_result;
        }
    };
}