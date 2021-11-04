#pragma once
#include "InstrumentationEngineApi.h"

/// <summary>
/// A class that demonstrates freeing instrumentation engine strings
/// using the c-style api from Instrumentation Engine.
/// </summary>
class CInstrStr
{
public:
    BSTR m_bstr = nullptr;
    CInstrStr() {}

    // non-copyable,moveable
    CInstrStr(CInstrStr& other) = delete;
    CInstrStr(CInstrStr&& other) noexcept
    {
        m_bstr = other.m_bstr;
        other.m_bstr = nullptr;
    }

    ~CInstrStr()
    {
        InstrumentationEngineApi::FreeString(m_bstr);
    }

    CInstrStr& operator=(CInstrStr& other) = delete;

    CInstrStr& operator=(CInstrStr&& other) noexcept
    {
        if (this != &other)
        {
            InstrumentationEngineApi::FreeString(m_bstr);
            m_bstr = other.m_bstr;
            other.m_bstr = nullptr;
        }

        return *this;
    }

    operator BSTR()
    {
        return m_bstr;
    }
};