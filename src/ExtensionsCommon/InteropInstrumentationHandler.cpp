// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"

#include "NativeInstanceMethodInstrumentationInfo.h"
#include "MethodInstrumentationInfoKey.h"
#include "MethodInstrumentationInfoCollection.h"

#include "InteropInstrumentationHandler.h"

namespace Agent
{
    namespace Interop
    {
        LPCWSTR WildcardName = L"*";

        CInteropInstrumentationHandler::CInteropInstrumentationHandler(
            const Instrumentation::CMethodInstrumentationInfoCollectionSptr& spMethodsToInstrument)
            : m_spMethodsToInstrument(spMethodsToInstrument)
        {
            IfTrueThrow(nullptr == spMethodsToInstrument, E_INVALIDARG);
            IfFailThrow(Create(m_spMethodInfoAdapter));
            IfFailThrow(Create(m_spNativeInstanceMethodsCodeInjector));
        }

        HRESULT CInteropInstrumentationHandler::BuildUpMembers()
        {
            IfFailRet(BuildUpObjPtr(m_spMethodInfoAdapter));
            IfFailRet(BuildUpObjPtr(m_spNativeInstanceMethodsCodeInjector));

            return S_OK;
        }

        CInteropInstrumentationHandler::~CInteropInstrumentationHandler()
        {
        }

        HRESULT CInteropInstrumentationHandler::AllowInline(
            _In_ const IMethodInfoSptr& sptrMethodInfoInlinee,
            _In_ const IMethodInfoSptr& sptrMethodInfoCaller)
        {
            HRESULT hr = S_OK;

			IfFailRetHresult(this->ShouldInstrument(sptrMethodInfoInlinee), hr);
			if (S_FALSE == hr)
            {
				IfFailRetHresult(this->ShouldInstrument(sptrMethodInfoCaller), hr);
			}

            return hr;
        }

        HRESULT CInteropInstrumentationHandler::ShouldInstrument(
            _In_ const IMethodInfoSptr& spMethodInfo)
        {
            HRESULT hr = S_OK;

            Instrumentation::Native::CNativeInstanceMethodInstrumentationInfoSptr spInstInfo;
            IfFailRetHresult(
                m_spMethodInfoAdapter->GetDataItem(
                spMethodInfo,
                spInstInfo), hr);

            if (S_FALSE == hr)
            {
                ATL::CComBSTR bstrFullMethodName;
                IfFailRet(spMethodInfo->GetFullName(&bstrFullMethodName));

                IModuleInfoSptr sptrModuleInfo;
                IfFailRet(spMethodInfo->GetModuleInfo(&sptrModuleInfo));

                ATL::CComBSTR bstrModuleName;
                IfFailRet(sptrModuleInfo->GetModuleName(&bstrModuleName));

                IAssemblyInfoSptr sptrAssemblyInfo;
                IfFailRet(sptrModuleInfo->GetAssemblyInfo(&sptrAssemblyInfo));

                ATL::CComBSTR bstrAssemblyName;
                IfFailRet(sptrAssemblyInfo->GetName(&bstrAssemblyName));

                ATL::CComPtr<IEnumMethodParameters> ptrMethodParameters;
                IfFailRet(spMethodInfo->GetParameters(&ptrMethodParameters));

                auto dwParametersCount = DWORD();
                IfFailRet(ptrMethodParameters->GetCount(&dwParametersCount));

                Instrumentation::CMethodInstrumentationInfoKey key(
                    bstrAssemblyName.m_str,
                    bstrModuleName.m_str,
                    bstrFullMethodName.m_str,
                    dwParametersCount);

                IfFailRetHresult(
                    m_spMethodsToInstrument->Get(
                    key,
                    spInstInfo), hr);

                if (S_OK != hr)
                {
                    Instrumentation::CMethodInstrumentationInfoKey wildcardAssemblyKey(
                        WildcardName,
                        WildcardName,
                        bstrFullMethodName.m_str,
                        dwParametersCount);

                    IfFailRetHresult(
                        m_spMethodsToInstrument->Get(
                        wildcardAssemblyKey,
                        spInstInfo), hr);
                }

                if (S_OK == hr)
                {
                    IfFailRet(m_spMethodInfoAdapter->SetDataItem(spMethodInfo, spInstInfo));
                }
            }

            return hr;
        }

        HRESULT CInteropInstrumentationHandler::Instrument(
            _In_ const IMethodInfoSptr& spMethodInfo)
        {
            HRESULT hr = S_OK;


            Instrumentation::Native::CNativeInstanceMethodInstrumentationInfoSptr spInstInfo;
            IfFailRetHresult(
                m_spMethodInfoAdapter->GetDataItem(
                spMethodInfo,
                spInstInfo), hr);

            if (S_OK == hr)
            {
                IfFailRet(
                    m_spNativeInstanceMethodsCodeInjector->Inject(
                    spMethodInfo,
                    spInstInfo));
            }


            return hr;
        }
    }
}