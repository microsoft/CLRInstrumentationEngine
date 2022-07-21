#include "InstanaInstrumentationMethod.h"

HRESULT CInstanaInstrumentationMethod::Initialize(_In_ IProfilerManager* pProfilerManager)
{
    CComPtr<IProfilerManagerLogging> spGlobalLogger;
    CComQIPtr<IProfilerManager4> pProfilerManager4 = pProfilerManager;
    pProfilerManager4->GetGlobalLoggingInstance(&spGlobalLogger);

    spGlobalLogger->LogDumpMessage(_T("Test module initialize"));

    if (m_bTestInstrumentationMethodLogging)
    {

        spGlobalLogger->LogDumpMessage(_T("<InstrumentationMethodLog>"));

        CComPtr<IProfilerManagerLogging> spLogger;
        pProfilerManager->GetLoggingInstance(&spLogger);
        spGlobalLogger->LogDumpMessage(_T("<Message>"));
        spLogger->LogMessage(_T("Message."));
        spGlobalLogger->LogDumpMessage(_T("</Message>"));

        spGlobalLogger->LogDumpMessage(_T("<Dump>"));
        spLogger->LogDumpMessage(_T("Success!"));
        spGlobalLogger->LogDumpMessage(_T("</Dump>"));

        spGlobalLogger->LogDumpMessage(_T("<Error>"));
        spLogger->LogError(_T("Error."));
        spGlobalLogger->LogDumpMessage(_T("</Error>"));
    }
    else if (m_bExceptionTrackingEnabled)
    {
        CComPtr<ICorProfilerInfo> pCorProfilerInfo;
        pProfilerManager->GetCorProfilerInfo((IUnknown**)&pCorProfilerInfo);
        pCorProfilerInfo->SetEventMask(COR_PRF_MONITOR_EXCEPTIONS | COR_PRF_ENABLE_STACK_SNAPSHOT);

        CComPtr<IProfilerManagerLogging> spLogger;
        pProfilerManager->GetLoggingInstance(&spLogger);
        spLogger->LogDumpMessage(_T("<ExceptionTrace>"));
    }

    return S_OK;
}