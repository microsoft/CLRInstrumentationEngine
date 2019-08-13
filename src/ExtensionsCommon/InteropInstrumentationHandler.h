// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "MethodInfoDataContainerAdatpter.h"
#include "NativeInstanceMethodsCodeInjector.h"
#include "MethodInstrumentationInfoCollection.h"

namespace Agent
{
    namespace Interop
    {
        extern LPCWSTR WildcardName;

        class CInteropInstrumentationHandler : public CProfilerHostServices
        {
            CInteropInstrumentationHandler(const CInteropInstrumentationHandler&) = delete;
            CInteropInstrumentationHandler& operator=(const CInteropInstrumentationHandler&) = delete;

            Instrumentation::CMethodInstrumentationInfoCollectionSptr m_spMethodsToInstrument;
            CMethodInfoDataContainerAdapterSptr	m_spMethodInfoAdapter;
            Instrumentation::CNativeInstanceMethodsCodeInjectorSPtr m_spNativeInstanceMethodsCodeInjector;
        public:
            CInteropInstrumentationHandler(
                const Instrumentation::CMethodInstrumentationInfoCollectionSptr& spMethodsToInstrument);
            virtual ~CInteropInstrumentationHandler();

            virtual HRESULT BuildUpMembers();

            HRESULT AllowInline(
                _In_ const IMethodInfoSptr& sptrMethodInfoInlinee,
                _In_ const IMethodInfoSptr& sptrMethodInfoCaller);

            HRESULT ShouldInstrument(
                _In_ const IMethodInfoSptr& sptrMethodInfo);

            HRESULT Instrument(
                _In_ const IMethodInfoSptr& sptrMethodInfo);
        }; // CInteropHandler

        typedef std::unique_ptr<CInteropInstrumentationHandler> CInteropInstrumentationHandlerUptr;

    }
}