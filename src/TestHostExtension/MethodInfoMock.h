// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
namespace TestsHostExtension
{

	class CMethodInfoMock
		: public IMethodInfo
		, public IDataContainer
	{
	private:
		ATL::CComBSTR m_name;
		ATL::CComBSTR m_fullName;

		IUnknown * m_itemInContainer;

		ULONG m_dwRef;

	public:
		CMethodInfoMock(const wchar_t* name, const wchar_t* fullName);
		~CMethodInfoMock();

		// Inherited via IMethodInfo
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject) override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override;
		virtual ULONG STDMETHODCALLTYPE Release(void) override;
		virtual HRESULT STDMETHODCALLTYPE  GetModuleInfo(IModuleInfo ** ppModuleInfo) override;
		virtual HRESULT STDMETHODCALLTYPE  GetName(BSTR * pbstrName) override;
		virtual HRESULT STDMETHODCALLTYPE  GetFullName(BSTR * pbstrFullName) override;
		virtual HRESULT STDMETHODCALLTYPE  GetInstructions(IInstructionGraph ** ppInstructionGraph) override;
		virtual HRESULT STDMETHODCALLTYPE  GetLocalVariables(ILocalVariableCollection ** ppLocalVariables) override;
		virtual HRESULT STDMETHODCALLTYPE  GetClassId(ClassID * pClassId) override;
		virtual HRESULT STDMETHODCALLTYPE  GetFunctionId(FunctionID * pFunctionID) override;
		virtual HRESULT STDMETHODCALLTYPE  GetMethodToken(mdToken * pToken) override;
		virtual HRESULT STDMETHODCALLTYPE  GetGenericParameterCount(DWORD * pCount) override;
		virtual HRESULT STDMETHODCALLTYPE  GetIsStatic(BOOL * pbValue) override;
		virtual HRESULT STDMETHODCALLTYPE  GetIsPublic(BOOL * pbValue) override;
		virtual HRESULT STDMETHODCALLTYPE  GetIsPrivate(BOOL * pbValue) override;
		virtual HRESULT STDMETHODCALLTYPE  GetIsPropertyGetter(BOOL * pbValue) override;
		virtual HRESULT STDMETHODCALLTYPE  GetIsPropertySetter(BOOL * pbValue) override;
		virtual HRESULT STDMETHODCALLTYPE  GetIsFinalizer(BOOL * pbValue) override;
		virtual HRESULT STDMETHODCALLTYPE  GetIsConstructor(BOOL * pbValue) override;
		virtual HRESULT STDMETHODCALLTYPE  GetIsStaticConstructor(BOOL * pbValue) override;
		virtual HRESULT STDMETHODCALLTYPE  GetParameters(IEnumMethodParameters ** pMethodArgs) override;
		virtual HRESULT STDMETHODCALLTYPE  GetDeclaringType(IType ** ppType) override;
		virtual HRESULT STDMETHODCALLTYPE  GetReturnType(IType ** ppType) override;
		virtual HRESULT STDMETHODCALLTYPE  GetCorSignature(_In_ DWORD cbBuffer, _Out_opt_ BYTE * pCorSignature, _Out_opt_ DWORD * pcbSignature) override;
		virtual HRESULT STDMETHODCALLTYPE  GetLocalVarSigToken(mdToken * pToken) override;
		virtual HRESULT STDMETHODCALLTYPE  SetLocalVarSigToken(mdToken token) override;
		virtual HRESULT STDMETHODCALLTYPE  GetAttributes(DWORD * pCorMethodAttr) override;
		virtual HRESULT STDMETHODCALLTYPE  GetRejitCodeGenFlags(DWORD * pRefitFlags) override;
		virtual HRESULT STDMETHODCALLTYPE  GetCodeRva(DWORD * pRva) override;
		virtual HRESULT STDMETHODCALLTYPE  MethodImplFlags(UINT * pCorMethodImpl) override;
		virtual HRESULT STDMETHODCALLTYPE  SetRejitCodeGenFlags(DWORD dwFlags) override;
		virtual HRESULT STDMETHODCALLTYPE  GetExceptionSection(IExceptionSection ** ppExceptionSection) override;
		virtual HRESULT STDMETHODCALLTYPE  GetInstructionFactory(IInstructionFactory ** ppInstructionFactory) override;
		virtual HRESULT STDMETHODCALLTYPE  GetRejitCount(DWORD * pdwRejitCount) override;
		virtual HRESULT STDMETHODCALLTYPE  GetMaxStack(DWORD * pMaxStack) override;
		virtual HRESULT STDMETHODCALLTYPE  GetSingleRetDefaultInstrumentation(ISingleRetDefaultInstrumentation ** ppSingleRetDefaultInstrumentation) override;

		// Inherited via IDataContainer
		virtual HRESULT STDMETHODCALLTYPE  SetDataItem(const GUID * componentId, const GUID * objectGuid, IUnknown * pDataItem) override;
		virtual HRESULT STDMETHODCALLTYPE  GetDataItem(const GUID * componentId, const GUID * objectGuid, IUnknown ** ppDataItem) override;
	};

}