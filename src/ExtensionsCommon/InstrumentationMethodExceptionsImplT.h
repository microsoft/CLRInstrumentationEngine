// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "InstrumentationEngineDefs.h"
#include "InstrumentationEnginePointerDefs.h"

#include "ProfilerHostServices.h"
#include "InstrumentationMethodUtils.h"

template<typename TThisType>
class CInstrumentationMethodExceptionsImplT
	: public IInstrumentationMethodExceptionEvents
	, virtual public CProfilerHostServices
{
	CInstrumentationMethodExceptionsImplT(
		const CInstrumentationMethodExceptionsImplT&) = delete;
public:
	CInstrumentationMethodExceptionsImplT() {}
	virtual ~CInstrumentationMethodExceptionsImplT();

	HRESULT STDMETHODCALLTYPE ExceptionCatcherEnter(
		/* [in] */ __RPC__in_opt IMethodInfo *pMethodInfo,
		/* [in] */ UINT_PTR objectId) override final;

	HRESULT STDMETHODCALLTYPE ExceptionCatcherLeave() override final;

	HRESULT STDMETHODCALLTYPE ExceptionSearchCatcherFound(
		/* [in] */ __RPC__in_opt IMethodInfo *pMethodInfo) override final;

	HRESULT STDMETHODCALLTYPE ExceptionSearchFilterEnter(
		/* [in] */ __RPC__in_opt IMethodInfo *pMethodInfo) override final;

	HRESULT STDMETHODCALLTYPE ExceptionSearchFilterLeave() override final;

	HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionEnter(
		/* [in] */ __RPC__in_opt IMethodInfo *pMethodInfo) override final;

	HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionLeave() override final;

	HRESULT STDMETHODCALLTYPE ExceptionThrown(
		/* [in] */ UINT_PTR thrownObjectId) override final;

	HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyEnter(
		/* [in] */ __RPC__in_opt IMethodInfo *pMethodInfo) override final;

	HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyLeave() override final;

	HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionEnter(
		/* [in] */ __RPC__in_opt IMethodInfo *pMethodInfo) override final;

	HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionLeave() override final;

protected:
	HRESULT InternalExceptionCatcherEnter(const IMethodInfoSptr& spMethodInfo, UINT_PTR objectId);

	HRESULT InternalExceptionCatcherLeave();

	HRESULT InternalExceptionSearchCatcherFound(const IMethodInfoSptr& spMethodInfo);

	HRESULT InternalExceptionSearchFilterEnter(const IMethodInfoSptr& spMethodInfo);

	HRESULT InternalExceptionSearchFilterLeave();

	HRESULT InternalExceptionSearchFunctionEnter(const IMethodInfoSptr& spMethodInfo);

	HRESULT InternalExceptionSearchFunctionLeave();

	HRESULT InternalExceptionThrown(UINT_PTR thrownObjectId);

	HRESULT InternalExceptionUnwindFinallyEnter(const IMethodInfoSptr& spMethodInfo);

	HRESULT InternalExceptionUnwindFinallyLeave();

	HRESULT InternalExceptionUnwindFunctionEnter(const IMethodInfoSptr& spMethodInfo);

	HRESULT InternalExceptionUnwindFunctionLeave();
};

template<typename TThisType>
CInstrumentationMethodExceptionsImplT<TThisType>::
	~CInstrumentationMethodExceptionsImplT()
{
}

template<typename TThisType>
HRESULT STDMETHODCALLTYPE CInstrumentationMethodExceptionsImplT<TThisType>::
	ExceptionCatcherEnter(
	/* [in] */ __RPC__in_opt IMethodInfo *pMethodInfo,
	/* [in] */ UINT_PTR objectId)
{
	IfTrueRet(nullptr == pMethodInfo, E_INVALIDARG);
	IMethodInfoSptr sptrMethodInfo(pMethodInfo);
	IfTrueRet(nullptr == sptrMethodInfo, E_INVALIDARG);

	HRESULT hr = S_OK;
	IfFailRetHresult(IsValidMethodInfo(sptrMethodInfo), hr);
	if (S_FALSE == hr)
	{
		TraceMsg(L"Method with invalid token detected");
		return hr;
	}

	IfFailRet(static_cast<TThisType*>(this)->InternalExceptionCatcherEnter(sptrMethodInfo, objectId));

	return S_OK;
}

template<typename TThisType>
HRESULT STDMETHODCALLTYPE CInstrumentationMethodExceptionsImplT<TThisType>::
	ExceptionCatcherLeave()
{
	IfFailRet(static_cast<TThisType*>(this)->InternalExceptionCatcherLeave());

	return S_OK;
}

template<typename TThisType>
HRESULT STDMETHODCALLTYPE CInstrumentationMethodExceptionsImplT<TThisType>::
	ExceptionSearchCatcherFound(
	/* [in] */ __RPC__in_opt IMethodInfo *pMethodInfo)
{
	IfTrueRet(nullptr == pMethodInfo, E_INVALIDARG);
	IMethodInfoSptr sptrMethodInfo(pMethodInfo);
	IfTrueRet(nullptr == sptrMethodInfo, E_INVALIDARG);

	HRESULT hr = S_OK;
	IfFailRetHresult(IsValidMethodInfo(sptrMethodInfo), hr);
	if (S_FALSE == hr)
	{
		TraceMsg(L"Method with invalid token detected");
		return hr;
	}

	IfFailRet(static_cast<TThisType*>(this)->InternalExceptionSearchCatcherFound(sptrMethodInfo));

	return S_OK;
}

template<typename TThisType>
HRESULT STDMETHODCALLTYPE CInstrumentationMethodExceptionsImplT<TThisType>::
	ExceptionSearchFilterEnter(
	/* [in] */ __RPC__in_opt IMethodInfo *pMethodInfo)
{
	IfTrueRet(nullptr == pMethodInfo, E_INVALIDARG);
	IMethodInfoSptr sptrMethodInfo(pMethodInfo);
	IfTrueRet(nullptr == sptrMethodInfo, E_INVALIDARG);

	HRESULT hr = S_OK;
	IfFailRetHresult(IsValidMethodInfo(sptrMethodInfo), hr);
	if (S_FALSE == hr)
	{
		TraceMsg(L"Method with invalid token detected");
		return hr;
	}

	IfFailRet(static_cast<TThisType*>(this)->InternalExceptionSearchFilterEnter(sptrMethodInfo));

	return S_OK;
}

template<typename TThisType>
HRESULT STDMETHODCALLTYPE CInstrumentationMethodExceptionsImplT<TThisType>::
	ExceptionSearchFilterLeave()
{
	IfFailRet(static_cast<TThisType*>(this)->InternalExceptionSearchFilterLeave());

	return S_OK;
}

template<typename TThisType>
HRESULT STDMETHODCALLTYPE CInstrumentationMethodExceptionsImplT<TThisType>::
	ExceptionSearchFunctionEnter(
	/* [in] */ __RPC__in_opt IMethodInfo *pMethodInfo)
{
	IfTrueRet(nullptr == pMethodInfo, E_INVALIDARG);
	IMethodInfoSptr sptrMethodInfo(pMethodInfo);
	IfTrueRet(nullptr == sptrMethodInfo, E_INVALIDARG);

	HRESULT hr = S_OK;
	IfFailRetHresult(IsValidMethodInfo(sptrMethodInfo), hr);
	if (S_FALSE == hr)
	{
		TraceMsg(L"Method with invalid token detected");
		return hr;
	}

	IfFailRet(static_cast<TThisType*>(this)->InternalExceptionSearchFunctionEnter(sptrMethodInfo));

	return S_OK;
}

template<typename TThisType>
HRESULT STDMETHODCALLTYPE CInstrumentationMethodExceptionsImplT<TThisType>::
	ExceptionSearchFunctionLeave()
{
	IfFailRet(static_cast<TThisType*>(this)->InternalExceptionSearchFunctionLeave());

	return S_OK;
}

template<typename TThisType>
HRESULT STDMETHODCALLTYPE CInstrumentationMethodExceptionsImplT<TThisType>::
	ExceptionThrown(
	/* [in] */ UINT_PTR thrownObjectId)
{
	IfFailRet(static_cast<TThisType*>(this)->InternalExceptionThrown(thrownObjectId));

	return S_OK;
}

template<typename TThisType>
HRESULT STDMETHODCALLTYPE CInstrumentationMethodExceptionsImplT<TThisType>::
	ExceptionUnwindFinallyEnter(
	/* [in] */ __RPC__in_opt IMethodInfo *pMethodInfo)
{
	IfTrueRet(nullptr == pMethodInfo, E_INVALIDARG);
	IMethodInfoSptr sptrMethodInfo(pMethodInfo);
	IfTrueRet(nullptr == sptrMethodInfo, E_INVALIDARG);

	HRESULT hr = S_OK;
	IfFailRetHresult(IsValidMethodInfo(sptrMethodInfo), hr);
	if (S_FALSE == hr)
	{
		TraceMsg(L"Method with invalid token detected");
		return hr;
	}

	IfFailRet(static_cast<TThisType*>(this)->InternalExceptionUnwindFinallyEnter(sptrMethodInfo));

	return S_OK;
}

template<typename TThisType>
HRESULT STDMETHODCALLTYPE CInstrumentationMethodExceptionsImplT<TThisType>::
	ExceptionUnwindFinallyLeave()
{
	IfFailRet(static_cast<TThisType*>(this)->InternalExceptionUnwindFinallyLeave());

	return S_OK;
}

template<typename TThisType>
HRESULT STDMETHODCALLTYPE CInstrumentationMethodExceptionsImplT<TThisType>::
	ExceptionUnwindFunctionEnter(
	/* [in] */ __RPC__in_opt IMethodInfo *pMethodInfo)
{
	IfTrueRet(nullptr == pMethodInfo, E_INVALIDARG);
	IMethodInfoSptr sptrMethodInfo(pMethodInfo);
	IfTrueRet(nullptr == sptrMethodInfo, E_INVALIDARG);

	HRESULT hr = S_OK;
	IfFailRetHresult(IsValidMethodInfo(sptrMethodInfo), hr);
	if (S_FALSE == hr)
	{
		TraceMsg(L"Method with invalid token detected");
		return hr;
	}

	IfFailRet(static_cast<TThisType*>(this)->InternalExceptionUnwindFinallyEnter(sptrMethodInfo));

	return S_OK;
}

template<typename TThisType>
HRESULT STDMETHODCALLTYPE CInstrumentationMethodExceptionsImplT<TThisType>::
	ExceptionUnwindFunctionLeave()
{
	IfFailRet(static_cast<TThisType*>(this)->InternalExceptionUnwindFunctionLeave());

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
/// Protected
//////////////////////////////////////////////////////////////////////////

template<typename TThisType>
HRESULT CInstrumentationMethodExceptionsImplT<TThisType>::
	InternalExceptionCatcherEnter(const IMethodInfoSptr&, UINT_PTR)
{
	return S_OK;
}

template<typename TThisType>
HRESULT CInstrumentationMethodExceptionsImplT<TThisType>::
	InternalExceptionCatcherLeave()
{
	return S_OK;
}

template<typename TThisType>
HRESULT CInstrumentationMethodExceptionsImplT<TThisType>::
	InternalExceptionSearchCatcherFound(const IMethodInfoSptr&)
{
	return S_OK;
}

template<typename TThisType>
HRESULT CInstrumentationMethodExceptionsImplT<TThisType>::
	InternalExceptionSearchFilterEnter(const IMethodInfoSptr&)
{
	return S_OK;
}

template<typename TThisType>
HRESULT CInstrumentationMethodExceptionsImplT<TThisType>::
	InternalExceptionSearchFilterLeave()
{
	return S_OK;
}

template<typename TThisType>
HRESULT CInstrumentationMethodExceptionsImplT<TThisType>::
	InternalExceptionSearchFunctionEnter(const IMethodInfoSptr&)
{
	return S_OK;
}

template<typename TThisType>
HRESULT CInstrumentationMethodExceptionsImplT<TThisType>::
	InternalExceptionSearchFunctionLeave()
{
	return S_OK;
}

template<typename TThisType>
HRESULT CInstrumentationMethodExceptionsImplT<TThisType>::
	InternalExceptionThrown(UINT_PTR thrownObjectId)
{
	return S_OK;
}

template<typename TThisType>
HRESULT CInstrumentationMethodExceptionsImplT<TThisType>::
	InternalExceptionUnwindFinallyEnter(const IMethodInfoSptr&)
{
	return S_OK;
}

template<typename TThisType>
HRESULT CInstrumentationMethodExceptionsImplT<TThisType>::
	InternalExceptionUnwindFinallyLeave()
{
	return S_OK;
}

template<typename TThisType>
HRESULT CInstrumentationMethodExceptionsImplT<TThisType>::
	InternalExceptionUnwindFunctionEnter(const IMethodInfoSptr&)
{
	return S_OK;
}

template<typename TThisType>
HRESULT CInstrumentationMethodExceptionsImplT<TThisType>::
	InternalExceptionUnwindFunctionLeave()
{
	return S_OK;
}