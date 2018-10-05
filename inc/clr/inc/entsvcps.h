

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __entsvcps_h__
#define __entsvcps_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IServicedComponentInfo_FWD_DEFINED__
#define __IServicedComponentInfo_FWD_DEFINED__
typedef interface IServicedComponentInfo IServicedComponentInfo;

#endif 	/* __IServicedComponentInfo_FWD_DEFINED__ */


#ifndef __IRemoteDispatch_FWD_DEFINED__
#define __IRemoteDispatch_FWD_DEFINED__
typedef interface IRemoteDispatch IRemoteDispatch;

#endif 	/* __IRemoteDispatch_FWD_DEFINED__ */


#ifndef __IManagedObject_FWD_DEFINED__
#define __IManagedObject_FWD_DEFINED__
typedef interface IManagedObject IManagedObject;

#endif 	/* __IManagedObject_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IServicedComponentInfo_INTERFACE_DEFINED__
#define __IServicedComponentInfo_INTERFACE_DEFINED__

/* interface IServicedComponentInfo */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IServicedComponentInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8165B19E-8D3A-4d0b-80C8-97DE310DB583")
    IServicedComponentInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetComponentInfo( 
            /* [out][in] */ __RPC__inout int *infoMask,
            /* [out] */ __RPC__deref_out_opt SAFEARRAY * *infoArray) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IServicedComponentInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IServicedComponentInfo * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IServicedComponentInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IServicedComponentInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponentInfo )( 
            __RPC__in IServicedComponentInfo * This,
            /* [out][in] */ __RPC__inout int *infoMask,
            /* [out] */ __RPC__deref_out_opt SAFEARRAY * *infoArray);
        
        END_INTERFACE
    } IServicedComponentInfoVtbl;

    interface IServicedComponentInfo
    {
        CONST_VTBL struct IServicedComponentInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IServicedComponentInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IServicedComponentInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IServicedComponentInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IServicedComponentInfo_GetComponentInfo(This,infoMask,infoArray)	\
    ( (This)->lpVtbl -> GetComponentInfo(This,infoMask,infoArray) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IServicedComponentInfo_INTERFACE_DEFINED__ */


#ifndef __IRemoteDispatch_INTERFACE_DEFINED__
#define __IRemoteDispatch_INTERFACE_DEFINED__

/* interface IRemoteDispatch */
/* [unique][dual][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IRemoteDispatch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6619a740-8154-43be-a186-0319578e02db")
    IRemoteDispatch : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RemoteDispatchAutoDone( 
            /* [in] */ __RPC__in BSTR s,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *pRetVal) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RemoteDispatchNotAutoDone( 
            /* [in] */ __RPC__in BSTR s,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *pRetVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IRemoteDispatchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IRemoteDispatch * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IRemoteDispatch * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IRemoteDispatch * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            __RPC__in IRemoteDispatch * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            __RPC__in IRemoteDispatch * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            __RPC__in IRemoteDispatch * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ __RPC__in_range(0,16384) UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IRemoteDispatch * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RemoteDispatchAutoDone )( 
            __RPC__in IRemoteDispatch * This,
            /* [in] */ __RPC__in BSTR s,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *pRetVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RemoteDispatchNotAutoDone )( 
            __RPC__in IRemoteDispatch * This,
            /* [in] */ __RPC__in BSTR s,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *pRetVal);
        
        END_INTERFACE
    } IRemoteDispatchVtbl;

    interface IRemoteDispatch
    {
        CONST_VTBL struct IRemoteDispatchVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRemoteDispatch_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRemoteDispatch_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRemoteDispatch_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRemoteDispatch_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IRemoteDispatch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IRemoteDispatch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IRemoteDispatch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IRemoteDispatch_RemoteDispatchAutoDone(This,s,pRetVal)	\
    ( (This)->lpVtbl -> RemoteDispatchAutoDone(This,s,pRetVal) ) 

#define IRemoteDispatch_RemoteDispatchNotAutoDone(This,s,pRetVal)	\
    ( (This)->lpVtbl -> RemoteDispatchNotAutoDone(This,s,pRetVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRemoteDispatch_INTERFACE_DEFINED__ */


#ifndef __IManagedObject_INTERFACE_DEFINED__
#define __IManagedObject_INTERFACE_DEFINED__

/* interface IManagedObject */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IManagedObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C3FCC19E-A970-11D2-8B5A-00A0C9B7C9C4")
    IManagedObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSerializedBuffer( 
            /* [out] */ __RPC__deref_out_opt BSTR *pBSTR) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetObjectIdentity( 
            /* [out] */ __RPC__deref_out_opt BSTR *pBSTRGUID,
            /* [out] */ __RPC__out int *AppDomainID,
            /* [out] */ __RPC__out int *pCCW) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IManagedObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IManagedObject * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IManagedObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IManagedObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSerializedBuffer )( 
            __RPC__in IManagedObject * This,
            /* [out] */ __RPC__deref_out_opt BSTR *pBSTR);
        
        HRESULT ( STDMETHODCALLTYPE *GetObjectIdentity )( 
            __RPC__in IManagedObject * This,
            /* [out] */ __RPC__deref_out_opt BSTR *pBSTRGUID,
            /* [out] */ __RPC__out int *AppDomainID,
            /* [out] */ __RPC__out int *pCCW);
        
        END_INTERFACE
    } IManagedObjectVtbl;

    interface IManagedObject
    {
        CONST_VTBL struct IManagedObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IManagedObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IManagedObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IManagedObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IManagedObject_GetSerializedBuffer(This,pBSTR)	\
    ( (This)->lpVtbl -> GetSerializedBuffer(This,pBSTR) ) 

#define IManagedObject_GetObjectIdentity(This,pBSTRGUID,AppDomainID,pCCW)	\
    ( (This)->lpVtbl -> GetObjectIdentity(This,pBSTRGUID,AppDomainID,pCCW) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IManagedObject_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     __RPC__in unsigned long *, __RPC__in BSTR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     __RPC__in unsigned long *, __RPC__in LPSAFEARRAY * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


