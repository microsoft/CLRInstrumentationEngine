// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

namespace MicrosoftInstrumentationEngine
{
    namespace Events
    {
        HRESULT SendJitCompleteEvent(IInstrumentationMethod* pInstrumentationMethod, IMethodJitInfo* pMethodJitInfo)
        {
            CComPtr<IInstrumentationMethodJitEvents2> pJitEvents2;
            CComPtr<IInstrumentationMethodJitEvents> pJitEvents;
            HRESULT hr;

            if (SUCCEEDED(pInstrumentationMethod->QueryInterface(__uuidof(IInstrumentationMethodJitEvents2), (void**)&pJitEvents2)))
            {
               IfFailRet(pJitEvents2->JitComplete(pMethodJitInfo));
            }
            else if (SUCCEEDED(pInstrumentationMethod->QueryInterface(__uuidof(IInstrumentationMethodJitEvents), (void**)&pJitEvents)))
            {
                HRESULT jitResult;
                FunctionID functionId;
                BOOL isRejit;
                IfFailRet(pMethodJitInfo->GetJitHR(&jitResult));
                IfFailRet(pMethodJitInfo->GetFunctionID(&functionId));
                IfFailRet(pMethodJitInfo->GetIsRejit(&isRejit));
                IfFailRet(pJitEvents->JitComplete(functionId, isRejit, jitResult));
            }

            return S_FALSE;
        }
    }
}