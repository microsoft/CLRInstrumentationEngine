// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"
#include "DebugLoggerSink.h"
#include "EventLoggerSink.h"
#include "FileLoggerSink.h"
#include "HostLoggerSink.h"
#include "LoggerService.h"

#include <filesystem>

namespace fs = std::experimental::filesystem;

namespace InstrumentationEngineLibTests
{
    class CTestEventLoggerSink :
        public CEventLoggerSink
    {
    private:
        std::vector<tstring> m_entries;

    public:
        void ClearEntries()
        {
            m_entries.clear();
        }

        const std::vector<tstring> GetEntries()
        {
            return m_entries;
        }

    protected:
        void LogEvent(const EventLogItem& tsEntry) override
        {
            // Sleep some arbitrary amount of time.
            Sleep(m_entries.size() % 25); // ms

            m_entries.push_back(tsEntry.tsEventLog);
        }
    };

    class CTestFileLoggerSink :
        public CFileLoggerSink
    {

    private:
        tstring m_tsPath;
        LoggingFlags m_flags;

    public:
        CTestFileLoggerSink(LoggingFlags flags, tstring& tsPath) :
            m_tsPath(tsPath),
            m_flags(flags)
        {
        }

    public:
        const tstring& GetPathIfExists()
        {
            return GetPathActual();
        }

    protected:
        HRESULT GetPathAndFlags(_Out_ tstring* ptsPath, _Out_ LoggingFlags* pFlags) override
        {
            if (!ptsPath || !pFlags)
            {
                return E_POINTER;
            }

            *ptsPath = m_tsPath;
            *pFlags = m_flags;

            return S_OK;
        }
    };

    class CTestLoggerService :
        public CLoggerService
    {
    private:
        LoggingFlags m_fileFlags;
        std::shared_ptr<CTestEventLoggerSink> m_pEventSink;
        std::shared_ptr<CTestFileLoggerSink> m_pFileSink;
        tstring m_tsFilePath;

    public:
        CTestLoggerService() :
            m_fileFlags(LoggingFlags_None)
        {
        }

    public:
        void ClearEventEntries() const
        {
            m_pEventSink->ClearEntries();
        }

        const tstring& GetFilePathIfExists() const
        {
            return m_pFileSink->GetPathIfExists();
        }

        const std::vector<tstring> GetEventEntries() const
        {
            return m_pEventSink->GetEntries();
        }

        void SetFileFlags(LoggingFlags fileFlags)
        {
            m_fileFlags = fileFlags;
        }

        void SetFilePath(const tstring& tsPath)
        {
            m_tsFilePath = tsPath;
        }

    protected:
        HRESULT CreateSinks(std::vector<std::shared_ptr<ILoggerSink>>& sinks) override
        {
            m_pEventSink = std::make_shared<CTestEventLoggerSink>();
            m_pFileSink = std::make_shared<CTestFileLoggerSink>(m_fileFlags, m_tsFilePath);

            sinks.push_back(std::make_shared<CDebugLoggerSink>());
            sinks.push_back(m_pEventSink);
            sinks.push_back(m_pFileSink);
            sinks.push_back(std::make_shared<CHostLoggerSink>());

            return S_OK;
        }
    };
}