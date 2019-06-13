// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once
typedef BYTE PROPVAR_PAD1;
typedef BYTE PROPVAR_PAD2;
typedef ULONG PROPVAR_PAD3;
typedef struct tagVersionedStream
    {
    GUID guidVersion;
    IStream *pStream;
    } 	VERSIONEDSTREAM;

typedef struct tagVersionedStream *LPVERSIONEDSTREAM;
typedef struct tagPROPVARIANT PROPVARIANT;

typedef struct tagPROPSPEC
    {
    ULONG ulKind;
    /* [switch_is] */ /* [switch_type] */ union
        {
        /* [case()] */ PROPID propid;
        /* [case()] */ LPOLESTR lpwstr;
        /* [default] */  /* Empty union arm */
        } 	DUMMYUNIONNAME;
    } 	PROPSPEC;

typedef struct tagCAC
    {
    ULONG cElems;
    /* [size_is] */ CHAR *pElems;
    } 	CAC;

typedef struct tagCAUB
    {
    ULONG cElems;
    /* [size_is] */ UCHAR *pElems;
    } 	CAUB;

typedef struct tagCAI
    {
    ULONG cElems;
    /* [size_is] */ SHORT *pElems;
    } 	CAI;

typedef struct tagCAUI
    {
    ULONG cElems;
    /* [size_is] */ USHORT *pElems;
    } 	CAUI;

typedef struct tagCAL
    {
    ULONG cElems;
    /* [size_is] */ LONG *pElems;
    } 	CAL;

typedef struct tagCAUL
    {
    ULONG cElems;
    /* [size_is] */ ULONG *pElems;
    } 	CAUL;

typedef struct tagCAFLT
    {
    ULONG cElems;
    /* [size_is] */ FLOAT *pElems;
    } 	CAFLT;

typedef struct tagCADBL
    {
    ULONG cElems;
    /* [size_is] */ DOUBLE *pElems;
    } 	CADBL;

typedef struct tagCACY
    {
    ULONG cElems;
    /* [size_is] */ CY *pElems;
    } 	CACY;

typedef struct tagCADATE
    {
    ULONG cElems;
    /* [size_is] */ DATE *pElems;
    } 	CADATE;

typedef struct tagCABSTR
    {
    ULONG cElems;
    /* [size_is] */ BSTR *pElems;
    } 	CABSTR;

typedef struct tagCABSTRBLOB
    {
    ULONG cElems;
    /* [size_is] */ BSTRBLOB *pElems;
    } 	CABSTRBLOB;

typedef struct tagCABOOL
    {
    ULONG cElems;
    /* [size_is] */ VARIANT_BOOL *pElems;
    } 	CABOOL;

typedef struct tagCASCODE
    {
    ULONG cElems;
    /* [size_is] */ SCODE *pElems;
    } 	CASCODE;

typedef struct tagCAPROPVARIANT
    {
    ULONG cElems;
    /* [size_is] */ PROPVARIANT *pElems;
    } 	CAPROPVARIANT;

typedef struct tagCAH
    {
    ULONG cElems;
    /* [size_is] */ LARGE_INTEGER *pElems;
    } 	CAH;

typedef struct tagCAUH
    {
    ULONG cElems;
    /* [size_is] */ ULARGE_INTEGER *pElems;
    } 	CAUH;

typedef struct tagCALPSTR
    {
    ULONG cElems;
    /* [size_is] */ LPSTR *pElems;
    } 	CALPSTR;

typedef struct tagCALPWSTR
    {
    ULONG cElems;
    /* [size_is] */ LPWSTR *pElems;
    } 	CALPWSTR;

typedef struct tagCAFILETIME
    {
    ULONG cElems;
    /* [size_is] */ FILETIME *pElems;
    } 	CAFILETIME;

typedef struct tagCACLIPDATA
    {
    ULONG cElems;
    /* [size_is] */ CLIPDATA *pElems;
    } 	CACLIPDATA;

typedef struct tagCACLSID
    {
    ULONG cElems;
    /* [size_is] */ CLSID *pElems;
    } 	CACLSID;

typedef struct tagSTATPROPSTG
    {
    LPOLESTR lpwstrName;
    PROPID propid;
    VARTYPE vt;
    } 	STATPROPSTG;

#ifndef MIDL_PASS
struct tagPROPVARIANT {
  union {
#endif
struct tag_inner_PROPVARIANT
    {
    VARTYPE vt;
    PROPVAR_PAD1 wReserved1;
    PROPVAR_PAD2 wReserved2;
    PROPVAR_PAD3 wReserved3;
    /* [switch_is] */ /* [switch_type] */ union
        {
        /* [case()] */  /* Empty union arm */
        /* [case()] */ CHAR cVal;
        /* [case()] */ UCHAR bVal;
        /* [case()] */ SHORT iVal;
        /* [case()] */ USHORT uiVal;
        /* [case()] */ LONG lVal;
        /* [case()] */ ULONG ulVal;
        /* [case()] */ INT intVal;
        /* [case()] */ UINT uintVal;
        /* [case()] */ LARGE_INTEGER hVal;
        /* [case()] */ ULARGE_INTEGER uhVal;
        /* [case()] */ FLOAT fltVal;
        /* [case()] */ DOUBLE dblVal;
        /* [case()] */ VARIANT_BOOL boolVal;
        /* [case()] */ _VARIANT_BOOL bool;
        /* [case()] */ SCODE scode;
        /* [case()] */ CY cyVal;
        /* [case()] */ DATE date;
        /* [case()] */ FILETIME filetime;
        /* [case()] */ CLSID *puuid;
        /* [case()] */ CLIPDATA *pclipdata;
        /* [case()] */ BSTR bstrVal;
        /* [case()] */ BSTRBLOB bstrblobVal;
        /* [case()] */ BLOB blob;
        /* [case()] */ LPSTR pszVal;
        /* [case()] */ LPWSTR pwszVal;
        /* [case()] */ IUnknown *punkVal;
        /* [case()] */ IDispatch *pdispVal;
        /* [case()] */ IStream *pStream;
        /* [case()] */ IStorage *pStorage;
        /* [case()] */ LPVERSIONEDSTREAM pVersionedStream;
        /* [case()] */ LPSAFEARRAY parray;
        /* [case()] */ CAC cac;
        /* [case()] */ CAUB caub;
        /* [case()] */ CAI cai;
        /* [case()] */ CAUI caui;
        /* [case()] */ CAL cal;
        /* [case()] */ CAUL caul;
        /* [case()] */ CAH cah;
        /* [case()] */ CAUH cauh;
        /* [case()] */ CAFLT caflt;
        /* [case()] */ CADBL cadbl;
        /* [case()] */ CABOOL cabool;
        /* [case()] */ CASCODE cascode;
        /* [case()] */ CACY cacy;
        /* [case()] */ CADATE cadate;
        /* [case()] */ CAFILETIME cafiletime;
        /* [case()] */ CACLSID cauuid;
        /* [case()] */ CACLIPDATA caclipdata;
        /* [case()] */ CABSTR cabstr;
        /* [case()] */ CABSTRBLOB cabstrblob;
        /* [case()] */ CALPSTR calpstr;
        /* [case()] */ CALPWSTR calpwstr;
        /* [case()] */ CAPROPVARIANT capropvar;
        /* [case()] */ CHAR *pcVal;
        /* [case()] */ UCHAR *pbVal;
        /* [case()] */ SHORT *piVal;
        /* [case()] */ USHORT *puiVal;
        /* [case()] */ LONG *plVal;
        /* [case()] */ ULONG *pulVal;
        /* [case()] */ INT *pintVal;
        /* [case()] */ UINT *puintVal;
        /* [case()] */ FLOAT *pfltVal;
        /* [case()] */ DOUBLE *pdblVal;
        /* [case()] */ VARIANT_BOOL *pboolVal;
        /* [case()] */ DECIMAL *pdecVal;
        /* [case()] */ SCODE *pscode;
        /* [case()] */ CY *pcyVal;
        /* [case()] */ DATE *pdate;
        /* [case()] */ BSTR *pbstrVal;
        /* [case()] */ IUnknown **ppunkVal;
        /* [case()] */ IDispatch **ppdispVal;
        /* [case()] */ LPSAFEARRAY *pparray;
        /* [case()] */ PROPVARIANT *pvarVal;
        } 	;
    } ;
#ifndef MIDL_PASS
    DECIMAL decVal;
  };
};

#endif

MIDL_INTERFACE("00000139-0000-0000-C000-000000000046")
IEnumSTATPROPSTG : public IUnknown
{
public:
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Next(
        /* [in] */ ULONG celt,
        /* [annotation][length_is][size_is][out] */
        _Out_writes_to_(celt, *pceltFetched)  STATPROPSTG *rgelt,
        /* [annotation][out] */
        _Out_opt_ _Deref_out_range_(0, celt)  ULONG *pceltFetched) = 0;

    virtual HRESULT STDMETHODCALLTYPE Skip(
        /* [in] */ ULONG celt) = 0;

    virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;

    virtual HRESULT STDMETHODCALLTYPE Clone(
        /* [out] */ __RPC__deref_out_opt IEnumSTATPROPSTG **ppenum) = 0;

};

