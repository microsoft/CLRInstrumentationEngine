#include "stdafx.h"
#include "Logging.h"
#include "../InstrumentationEngine.Api/InstrumentationEngine.h"

CInitOnce CLogging::s_initialize([]() { return InitializeCore(); });
CSingleton<CLoggerService> CLogging::s_logger;
volatile LONG CLogging::s_initCount = 0;

// static
bool CLogging::AllowLogEntry(_In_ LoggingFlags flags)
{
    return s_logger.Get()->AllowLogEntry(flags);
}

// static
HRESULT CLogging::GetLoggingFlags(_Out_ LoggingFlags* pLoggingFlags)
{
    return s_logger.Get()->GetLoggingFlags(pLoggingFlags);
}

// static
void CLogging::LogMessage(_In_ const WCHAR* wszMessage, ...)
{
    va_list argptr;
    va_start(argptr, wszMessage);
    s_logger.Get()->LogMessage(wszMessage, argptr);
    va_end(argptr);
}

// static
void CLogging::LogError(_In_ const WCHAR* wszError, ...)
{
    va_list argptr;
    va_start(argptr, wszError);
    s_logger.Get()->LogError(wszError, argptr);
    va_end(argptr);
}

// static
void CLogging::LogDumpMessage(_In_ const WCHAR* wszMessage, ...)
{
    va_list argptr;
    va_start(argptr, wszMessage);
    s_logger.Get()->LogDumpMessage(wszMessage, argptr);
    va_end(argptr);
}

// static
HRESULT CLogging::Initialize()
{
    HRESULT hr = s_initialize.Get();

    if (s_initialize.IsSuccessful())
    {
        InterlockedIncrement(&s_initCount);
    }

    return hr;
}

// static
HRESULT CLogging::InitializeCore()
{
    return s_logger.Get()->Initialize();
}

// static
void CLogging::SetLogToDebugPort(_In_ bool enable)
{
    s_logger.Get()->SetLogToDebugPort(enable);
}

// static
HRESULT CLogging::SetLoggingFlags(_In_ LoggingFlags loggingFlags)
{
    return s_logger.Get()->SetLoggingFlags(loggingFlags);
}

// static
HRESULT CLogging::SetLoggingHost(_In_ IProfilerManagerLoggingHost* pLoggingHost)
{
    return s_logger.Get()->SetLoggingHost(pLoggingHost);
}

// static
HRESULT CLogging::Shutdown()
{
    if (!s_initialize.IsSuccessful())
    {
        return E_UNEXPECTED;
    }

    if (0 == InterlockedDecrement(&s_initCount))
    {
        return s_logger.Get()->Shutdown();
    }
    return S_OK;
}

CLogging::XmlDumpHelper::XmlDumpHelper(const WCHAR* tag, const unsigned int indent)
{
    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }
    m_tag = tag;

    for (unsigned int i = 0; i < indent; ++i)
    {
        m_indent += _T("    ");
    }
    m_childrenIndent = m_indent + _T("    ");
    m_result << m_indent << _T("<") << m_tag << _T(">") << _T("\r\n");
}

CLogging::XmlDumpHelper::~XmlDumpHelper()
{
    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }

    m_result << m_indent << _T("</") << m_tag << _T(">") << _T("\r\n") << std::flush;

    CLogging::LogDumpMessage(m_result.str().c_str());
}

void CLogging::XmlDumpHelper::WriteStringNode(const WCHAR* name, const WCHAR* value)
{
    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }
    m_result << m_childrenIndent << _T("<") << name << _T(">") << value << _T("</") << name << _T(">") << _T("\r\n");
}

void CLogging::XmlDumpHelper::WriteUlongNode(const WCHAR* name, const ULONG32 value)
{
    if (!AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }

    m_result << showbase // show the 0x prefix
        << internal // fill between the prefix and the number
        << setfill(_T('0')); // fill with 0s

    m_result << m_childrenIndent << _T('<') << name << _T('>') << hex << setw(10) << value << _T("</") << name << _T('>') << _T("\r\n");
}