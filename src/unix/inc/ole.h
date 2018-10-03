#pragma once


#ifdef _OLEAUT32_
#define WINOLEAUTAPI        STDAPI
#define WINOLEAUTAPI_(type) STDAPI_(type)
#else
#define WINOLEAUTAPI        EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define WINOLEAUTAPI_(type) EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#endif
#define WINOLEAPI        EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE

#define VARCMP_LT   0
#define VARCMP_EQ   1
#define VARCMP_GT   2
#define VARCMP_NULL 3

#define DISPATCH_METHOD         0x1
#define DISPATCH_PROPERTYGET    0x2
#define DISPATCH_PROPERTYPUT    0x4
#define DISPATCH_PROPERTYPUTREF 0x8

#define VARIANT_NOVALUEPROP      0x01
typedef LONG LSTATUS;

// TODO:
extern const CLSID CLSID_StdGlobalInterfaceTable;


// from objidl.h

MIDL_INTERFACE("0000010c-0000-0000-C000-000000000046")
IPersist : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetClassID( 
        /* [out] */ __RPC__out CLSID *pClassID) = 0;
    
};
typedef IStream *LPSTREAM;

MIDL_INTERFACE("00000109-0000-0000-C000-000000000046")
IPersistStream : public IPersist
{
public:
    virtual HRESULT STDMETHODCALLTYPE IsDirty( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE Load( 
        /* [unique][in] */ __RPC__in_opt IStream *pStm) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE Save( 
        /* [unique][in] */ __RPC__in_opt IStream *pStm,
        /* [in] */ BOOL fClearDirty) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetSizeMax( 
        /* [out] */ __RPC__out ULARGE_INTEGER *pcbSize) = 0;
    
};
typedef IPersistStream *LPPERSISTSTREAM;

MIDL_INTERFACE("7FD52380-4E07-101B-AE2D-08002B2EC713")
IPersistStreamInit : public IPersist
{
public:
    virtual HRESULT STDMETHODCALLTYPE IsDirty( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE Load( 
        /* [in] */ __RPC__in_opt LPSTREAM pStm) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE Save( 
        /* [in] */ __RPC__in_opt LPSTREAM pStm,
        /* [in] */ BOOL fClearDirty) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetSizeMax( 
        /* [out] */ __RPC__out ULARGE_INTEGER *pCbSize) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE InitNew( void) = 0;
};

MIDL_INTERFACE("618736e0-3c3d-11cf-810c-00aa00389b71")
IAccessible : public IDispatch
{
public:
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accParent( 
        /* [retval][out] */ __RPC__deref_out_opt IDispatch **ppdispParent) = 0;
    
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accChildCount( 
        /* [retval][out] */ __RPC__out long *pcountChildren) = 0;
    
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accChild( 
        /* [in] */ VARIANT varChild,
        /* [retval][out] */ __RPC__deref_out_opt IDispatch **ppdispChild) = 0;
    
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accName( 
        /* [optional][in] */ VARIANT varChild,
        /* [retval][out] */ __RPC__deref_out_opt BSTR *pszName) = 0;
    
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accValue( 
        /* [optional][in] */ VARIANT varChild,
        /* [retval][out] */ __RPC__deref_out_opt BSTR *pszValue) = 0;
    
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accDescription( 
        /* [optional][in] */ VARIANT varChild,
        /* [retval][out] */ __RPC__deref_out_opt BSTR *pszDescription) = 0;
    
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accRole( 
        /* [optional][in] */ VARIANT varChild,
        /* [retval][out] */ __RPC__out VARIANT *pvarRole) = 0;
    
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accState( 
        /* [optional][in] */ VARIANT varChild,
        /* [retval][out] */ __RPC__out VARIANT *pvarState) = 0;
    
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accHelp( 
        /* [optional][in] */ VARIANT varChild,
        /* [retval][out] */ __RPC__deref_out_opt BSTR *pszHelp) = 0;
    
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accHelpTopic( 
        /* [out] */ __RPC__deref_out_opt BSTR *pszHelpFile,
        /* [optional][in] */ VARIANT varChild,
        /* [retval][out] */ __RPC__out long *pidTopic) = 0;
    
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accKeyboardShortcut( 
        /* [optional][in] */ VARIANT varChild,
        /* [retval][out] */ __RPC__deref_out_opt BSTR *pszKeyboardShortcut) = 0;
    
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accFocus( 
        /* [retval][out] */ __RPC__out VARIANT *pvarChild) = 0;
    
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accSelection( 
        /* [retval][out] */ __RPC__out VARIANT *pvarChildren) = 0;
    
    virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accDefaultAction( 
        /* [optional][in] */ VARIANT varChild,
        /* [retval][out] */ __RPC__deref_out_opt BSTR *pszDefaultAction) = 0;
    
    virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE accSelect( 
        /* [in] */ long flagsSelect,
        /* [optional][in] */ VARIANT varChild) = 0;
    
    virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE accLocation( 
        /* [out] */ __RPC__out long *pxLeft,
        /* [out] */ __RPC__out long *pyTop,
        /* [out] */ __RPC__out long *pcxWidth,
        /* [out] */ __RPC__out long *pcyHeight,
        /* [optional][in] */ VARIANT varChild) = 0;
    
    virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE accNavigate( 
        /* [in] */ long navDir,
        /* [optional][in] */ VARIANT varStart,
        /* [retval][out] */ __RPC__out VARIANT *pvarEndUpAt) = 0;
    
    virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE accHitTest( 
        /* [in] */ long xLeft,
        /* [in] */ long yTop,
        /* [retval][out] */ __RPC__out VARIANT *pvarChild) = 0;
    
    virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE accDoDefaultAction( 
        /* [optional][in] */ VARIANT varChild) = 0;
    
    virtual /* [id][propput][hidden] */ HRESULT STDMETHODCALLTYPE put_accName( 
        /* [optional][in] */ VARIANT varChild,
        /* [in] */ __RPC__in BSTR szName) = 0;
    
    virtual /* [id][propput][hidden] */ HRESULT STDMETHODCALLTYPE put_accValue( 
        /* [optional][in] */ VARIANT varChild,
        /* [in] */ __RPC__in BSTR szValue) = 0;
    
};

MIDL_INTERFACE("00020404-0000-0000-C000-000000000046")
IEnumVARIANT : public IUnknown
{
public:
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Next( 
        /* [in] */ ULONG celt,
        /* [length_is][size_is][out] */ VARIANT *rgVar,
        /* [out] */ ULONG *pCeltFetched) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE Skip( 
        /* [in] */ ULONG celt) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE Clone( 
        /* [out] */ __RPC__deref_out_opt IEnumVARIANT **ppEnum) = 0;
    
};

MIDL_INTERFACE("00000146-0000-0000-C000-000000000046")
IGlobalInterfaceTable : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE RegisterInterfaceInGlobal( 
        /* [annotation][in] */ 
        _In_  IUnknown *pUnk,
        /* [annotation][in] */ 
        _In_  REFIID riid,
        /* [annotation][out] */ 
        _Out_  DWORD *pdwCookie) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE RevokeInterfaceFromGlobal( 
        /* [annotation][in] */ 
        _In_  DWORD dwCookie) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetInterfaceFromGlobal( 
        /* [annotation][in] */ 
        _In_  DWORD dwCookie,
        /* [annotation][in] */ 
        _In_  REFIID riid,
        /* [annotation][iid_is][out] */ 
        _Outptr_  void **ppv) = 0;
    
};

typedef enum tagCOWAIT_FLAGS { 
  COWAIT_WAITALL                   = 1,
  COWAIT_ALERTABLE                 = 2,
  COWAIT_INPUTAVAILABLE            = 4,
  COWAIT_DISPATCH_CALLS            = 8,
  COWAIT_DISPATCH_WINDOW_MESSAGES  = 0X10
} COWAIT_FLAGS;

STDAPI VarBstrCat(_In_ BSTR bstrLeft, _In_ BSTR bstrRight, _Out_ LPBSTR pbstrResult);
STDAPI VarBstrCmp(_In_ BSTR bstrLeft, _In_ BSTR bstrRight, _In_ LCID lcid, _In_ ULONG dwFlags); // dwFlags passed to CompareString
WINOLEAUTAPI VectorFromBstr (_In_ BSTR bstr, _Outptr_ SAFEARRAY ** ppsa);
WINOLEAUTAPI BstrFromVector (_In_ SAFEARRAY *psa, _Out_ BSTR *pbstr);
WINOLEAUTAPI SafeArrayCopy(_In_ SAFEARRAY * psa, _Outptr_ SAFEARRAY ** ppsaOut);
WINOLEAUTAPI VariantCopy(_Inout_ VARIANTARG * pvargDest, _In_ const VARIANTARG * pvargSrc);
WINOLEAUTAPI VariantChangeType(_Inout_ VARIANTARG * pvargDest, _In_ const VARIANTARG * pvarSrc, _In_ USHORT wFlags, _In_ VARTYPE vt);
WINOLEAPI OleSaveToStream(IN LPPERSISTSTREAM pPStm, IN LPSTREAM pStm);
WINOLEAPI WriteClassStm(IN LPSTREAM pStm, IN REFCLSID rclsid);
STDAPI VarCmp(_In_ LPVARIANT pvarLeft, _In_ LPVARIANT pvarRight, _In_ LCID lcid, _In_ ULONG dwFlags);
STDAPI_(void) CoUninitialize();
WINOLEAPI CoInitializeEx(_In_opt_ LPVOID pvReserved, _In_ DWORD dwCoInit);

// TODO: Remove these.
WINOLEAPI ReadClassStm(IN LPSTREAM pStm, OUT CLSID FAR* pclsid);
WINOLEAPI CoRegisterClassObject(_In_ REFCLSID rclsid, _In_ LPUNKNOWN pUnk, _In_ DWORD dwClsContext, _In_ DWORD flags, _Out_ LPDWORD lpdwRegister);
WINOLEAPI CoRevokeClassObject(_In_ DWORD dwRegister);
WINOLEAPI CoResumeClassObjects(void);
WINOLEAPI CoSuspendClassObjects(void);
WINOLEAPI CoInitialize(_In_opt_ LPVOID pvReserved);
WINOLEAPI CoMarshalInterThreadInterfaceInStream(REFIID riid, LPUNKNOWN pUnk, LPSTREAM * ppStm);
WINOLEAPI CoWaitForMultipleHandles(DWORD dwFlags, DWORD dwTimeout, ULONG cHandles, LPHANDLE pHandles, LPDWORD lpdwindex);