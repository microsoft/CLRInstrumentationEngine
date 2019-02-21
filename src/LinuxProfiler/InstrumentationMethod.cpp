//// Copyright (c) Microsoft Corporation. All rights reserved.
//// 
//
//#include "InstrumentationMethod.h"
//
//HRESULT CInstrumentationMethod::Initialize(IProfilerManager* pProfilerManager)
//{
//    CComPtr<ICorProfilerInfo> pCorProfilerInfo;
//    pProfilerManager->GetCorProfilerInfo((IUnknown**)&pCorProfilerInfo);
//    pCorProfilerInfo->SetEventMask(COR_PRF_MONITOR_EXCEPTIONS | COR_PRF_ENABLE_STACK_SNAPSHOT);
//    
//    return S_OK;
//}