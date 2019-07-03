// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"
#include "../InstrumentationEngine.Lib/ImplQueryInterface.h"
#include "../InstrumentationEngine.Lib/refcount.h"

namespace InstrumentationEngineLibTests
{
    class CTestLoggingHost :
        public IProfilerManagerLoggingHost,
        public CModuleRefCount
    {
    private:
        static const tstring CarriageReturnNewLine;

    public:
        std::vector<tstring> m_dumps;
        std::vector<tstring> m_errors;
        std::vector<tstring> m_messages;

        // IUnknown Members
    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CLoggingHost);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CLoggingHost);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IProfilerManagerLoggingHost*>(this),
                riid,
                ppvObject
            );
        }

        // IProfilerManagerLoggintHost Members
    public:
        STDMETHOD(LogMessage)(const WCHAR *wszMessage)
        {
            // Ignore carriage return + new line
            if (CarriageReturnNewLine.compare(wszMessage))
            {
                m_messages.emplace_back(wszMessage);
            }
            return S_OK;
        }

        STDMETHOD(LogError)(const WCHAR *wszError)
        {
            // Ignore carriage return + new line
            if (CarriageReturnNewLine.compare(wszError))
            {
                m_errors.emplace_back(wszError);
            }
            return S_OK;
        }

        STDMETHOD(LogDumpMessage)(const WCHAR *wszMessage)
        {
            // Ignore carriage return + new line
            if (CarriageReturnNewLine.compare(wszMessage))
            {
                m_dumps.emplace_back(wszMessage);
            }
            return S_OK;
        }

    public:
        void Reset()
        {
            m_dumps.clear();
            m_errors.clear();
            m_messages.clear();
        }
    };

    // static
    const tstring CTestLoggingHost::CarriageReturnNewLine = L"\r\n";
}