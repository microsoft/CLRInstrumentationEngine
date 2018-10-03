// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "MethodInfoMock.h"

namespace TestsHostExtension
{

	CMethodInfoMock::CMethodInfoMock(const wchar_t* name, const wchar_t* fullName)
		: m_name(name)
		, m_fullName(fullName)
		, m_dwRef(0)
		, m_itemInContainer(nullptr)
	{
	}


	CMethodInfoMock::~CMethodInfoMock()
	{
	}

	HRESULT STDMETHODCALLTYPE CMethodInfoMock::QueryInterface(REFIID riid, void ** ppvObject)
	{
		if (riid == __uuidof(IDataContainer))
		{
			AddRef();
			*ppvObject = static_cast<IDataContainer*>(this);
			return S_OK;
		}

		return E_NOTIMPL;
	}

	ULONG STDMETHODCALLTYPE CMethodInfoMock::AddRef(void)
	{
		return InterlockedIncrement(&m_dwRef);
	}

	ULONG STDMETHODCALLTYPE CMethodInfoMock::Release(void)
	{
		ULONG result = InterlockedDecrement(&m_dwRef);
		if (result == 0)
			delete this;
		return result;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetModuleInfo(IModuleInfo ** ppModuleInfo)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetName(BSTR * pbstrName)
	{
		ATL::CComBSTR bstrName = m_name;
		//*pbstrName = bstrName.Detach();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetFullName(BSTR * pbstrFullName)
	{
		ATL::CComBSTR bstrFullName = m_fullName;
		*pbstrFullName = bstrFullName.Detach();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetInstructions(IInstructionGraph ** ppInstructionGraph)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetLocalVariables(ILocalVariableCollection ** ppLocalVariables)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetClassId(ClassID * pClassId)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetFunctionId(FunctionID * pFunctionID)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetMethodToken(mdToken * pToken)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetGenericParameterCount(DWORD * pCount)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetIsStatic(BOOL * pbValue)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetIsPublic(BOOL * pbValue)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetIsPrivate(BOOL * pbValue)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetIsPropertyGetter(BOOL * pbValue)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetIsPropertySetter(BOOL * pbValue)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetIsFinalizer(BOOL * pbValue)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetIsConstructor(BOOL * pbValue)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetIsStaticConstructor(BOOL * pbValue)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetParameters(IEnumMethodParameters ** pMethodArgs)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetDeclaringType(IType ** ppType)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetReturnType(IType ** ppType)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetCorSignature(DWORD cbBuffer, BYTE * pCorSignature, DWORD * pcbSignature)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetLocalVarSigToken(mdToken * pToken)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::SetLocalVarSigToken(mdToken token)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetAttributes(DWORD * pCorMethodAttr)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetRejitCodeGenFlags(DWORD * pRefitFlags)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetCodeRva(DWORD * pRva)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::MethodImplFlags(UINT * pCorMethodImpl)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::SetRejitCodeGenFlags(DWORD dwFlags)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetExceptionSection(IExceptionSection ** ppExceptionSection)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetInstructionFactory(IInstructionFactory ** ppInstructionFactory)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetRejitCount(DWORD * pdwRejitCount)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetMaxStack(DWORD * pMaxStack)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::GetSingleRetDefaultInstrumentation(ISingleRetDefaultInstrumentation ** ppSingleRetDefaultInstrumentation)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  CMethodInfoMock::SetDataItem(const GUID * componentId, const GUID * objectGuid, IUnknown * pDataItem)
	{
		m_itemInContainer = pDataItem;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE CMethodInfoMock::GetDataItem(const GUID * componentId, const GUID * objectGuid, IUnknown ** ppDataItem)
	{
		if (m_itemInContainer == nullptr)
		{
			return E_FAIL;
		}

		*ppDataItem = m_itemInContainer;
		return S_OK;
	}
}