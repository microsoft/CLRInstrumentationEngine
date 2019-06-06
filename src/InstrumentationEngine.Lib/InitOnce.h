#pragma once

#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy can be used
#include <atomic>
#pragma warning(pop)
#include <functional>
#include <atlsync.h>
#include "CriticalSectionHolder.h"

namespace MicrosoftInstrumentationEngine
{
    class CInitOnce
    {
    private:
        std::atomic_bool m_isCreated;
        CCriticalSection m_cs;
        std::function<HRESULT()> m_func;
        HRESULT m_result;

    public:
        CInitOnce(std::function<HRESULT()> func) : m_isCreated(false), m_func(func), m_result(0) { }

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

        bool IsSuccessful()
        {
            return m_isCreated.load(std::memory_order_relaxed) && SUCCEEDED(m_result);
        }
    };
}