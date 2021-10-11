#pragma once

#include "stdafx.h"
#include "../Common.Headers/tstring.h"

class CClrieString
{
private:
    CComPtr<IProfilerStringManager> m_pStringManager;

public:
    BSTR m_bstr;
    CClrieString(IProfilerStringManager* pStringManager) : m_bstr(nullptr), m_pStringManager(pStringManager) {}

    // non-copyable,moveable
    CClrieString(CClrieString& other) = delete;
    CClrieString(CClrieString&& other) noexcept
    {
        m_bstr = other.m_bstr;
        m_pStringManager = other.m_pStringManager;

        other.m_bstr = nullptr;
        other.m_pStringManager = nullptr;
    }

    CClrieString& operator=(CClrieString& other) = delete;

    CClrieString& operator=(CClrieString&& other) noexcept
    {
        if (this != &other)
        {
            m_pStringManager->FreeString(m_bstr);
            m_bstr = other.m_bstr;

            other.m_bstr = nullptr;
            other.m_pStringManager = nullptr;
        }

        return *this;
    }

    operator BSTR()
    {
        return m_bstr;
    }

    ~CClrieString()
    {
        if (m_pStringManager != nullptr)
        {
            m_pStringManager->FreeString(m_bstr);
        }
    }

    const BSTR operator()()
    {
        return m_bstr;
    }

    BSTR BStr()
    {
        return m_bstr;
    }
};


