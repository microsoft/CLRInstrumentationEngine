// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"

namespace MicrosoftInstrumentationEngine
{
    class CLoggingWrapper :
        public IProfilerManagerLogging,
        public CModuleRefCount
    {
    private:
        CInitOnce m_initialize;

    public:
        CLoggingWrapper() :
            m_initialize([=]() { return InitializeCore(); })
        {
        }

        ~CLoggingWrapper()
        {
            if (m_initialize.IsSuccessful())
            {
                CLogging::Shutdown();
            }
        }

        // IUnknown Members
    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CLoggingWrapper);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CLoggingWrapper);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IProfilerManagerLogging*>(this),
                riid,
                ppvObject
            );
        }

        // IProfilerManagerLogging Members
    public:
        STDMETHOD(LogMessage)(_In_ const WCHAR* wszMessage) override
        {
            IfNotInitRetUnexpected(m_initialize);

            CLogging::LogMessage(wszMessage);
            return S_OK;
        }

        STDMETHOD(LogError)(_In_ const WCHAR* wszError) override
        {
            IfNotInitRetUnexpected(m_initialize);

            CLogging::LogError(wszError);
            return S_OK;
        }

        STDMETHOD(LogDumpMessage)(_In_ const WCHAR* wszMessage) override
        {
            IfNotInitRetUnexpected(m_initialize);

            CLogging::LogDumpMessage(wszMessage);
            return S_OK;
        }

        STDMETHOD(EnableDiagnosticLogToDebugPort)(_In_ BOOL enable) override
        {
            IfNotInitRetUnexpected(m_initialize);

            CLogging::SetLogToDebugPort(enable != 0);
            return S_OK;
        }

        STDMETHOD(GetLoggingFlags)(_Out_ LoggingFlags* pLoggingFlags) override
        {
            IfNotInitRetUnexpected(m_initialize);

            return CLogging::GetLoggingFlags(pLoggingFlags);
        }

        STDMETHOD(SetLoggingFlags)(_In_ LoggingFlags loggingFlags) override
        {
            IfNotInitRetUnexpected(m_initialize);

            return CLogging::SetLoggingFlags(loggingFlags);
        }

    public:
        HRESULT Initialize()
        {
            return m_initialize.Get();
        }

    private:
        static HRESULT InitializeCore()
        {
            return CLogging::Initialize();
        }
    };
}