

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __CLRPrivAppXHosting_h__
#define __CLRPrivAppXHosting_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICLRPrivAppXRuntime_FWD_DEFINED__
#define __ICLRPrivAppXRuntime_FWD_DEFINED__
typedef interface ICLRPrivAppXRuntime ICLRPrivAppXRuntime;

#endif 	/* __ICLRPrivAppXRuntime_FWD_DEFINED__ */


#ifndef __ICLRPrivAppXDomain_FWD_DEFINED__
#define __ICLRPrivAppXDomain_FWD_DEFINED__
typedef interface ICLRPrivAppXDomain ICLRPrivAppXDomain;

#endif 	/* __ICLRPrivAppXDomain_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_CLRPrivAppXHosting_0000_0000 */
/* [local] */ 





extern RPC_IF_HANDLE __MIDL_itf_CLRPrivAppXHosting_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_CLRPrivAppXHosting_0000_0000_v0_0_s_ifspec;

#ifndef __ICLRPrivAppXRuntime_INTERFACE_DEFINED__
#define __ICLRPrivAppXRuntime_INTERFACE_DEFINED__

/* interface ICLRPrivAppXRuntime */
/* [object][local][version][uuid] */ 


EXTERN_C const IID IID_ICLRPrivAppXRuntime;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6D2DF5A4-FA3A-4481-8BA0-0422FD21720F")
    ICLRPrivAppXRuntime : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InitializeAppXDomain( 
            /* [in] */ LPCWSTR wzFriendlyName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAppXDomain( 
            /* [in] */ REFIID riidDomain,
            /* [out] */ LPVOID *ppvDomain) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICLRPrivAppXRuntimeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICLRPrivAppXRuntime * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICLRPrivAppXRuntime * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICLRPrivAppXRuntime * This);
        
        HRESULT ( STDMETHODCALLTYPE *InitializeAppXDomain )( 
            ICLRPrivAppXRuntime * This,
            /* [in] */ LPCWSTR wzFriendlyName);
        
        HRESULT ( STDMETHODCALLTYPE *GetAppXDomain )( 
            ICLRPrivAppXRuntime * This,
            /* [in] */ REFIID riidDomain,
            /* [out] */ LPVOID *ppvDomain);
        
        END_INTERFACE
    } ICLRPrivAppXRuntimeVtbl;

    interface ICLRPrivAppXRuntime
    {
        CONST_VTBL struct ICLRPrivAppXRuntimeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICLRPrivAppXRuntime_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICLRPrivAppXRuntime_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICLRPrivAppXRuntime_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICLRPrivAppXRuntime_InitializeAppXDomain(This,wzFriendlyName)	\
    ( (This)->lpVtbl -> InitializeAppXDomain(This,wzFriendlyName) ) 

#define ICLRPrivAppXRuntime_GetAppXDomain(This,riidDomain,ppvDomain)	\
    ( (This)->lpVtbl -> GetAppXDomain(This,riidDomain,ppvDomain) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICLRPrivAppXRuntime_INTERFACE_DEFINED__ */


#ifndef __ICLRPrivAppXDomain_INTERFACE_DEFINED__
#define __ICLRPrivAppXDomain_INTERFACE_DEFINED__

/* interface ICLRPrivAppXDomain */
/* [object][local][version][uuid] */ 


EXTERN_C const IID IID_ICLRPrivAppXDomain;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6633398E-823D-4361-B30E-824043BD4686")
    ICLRPrivAppXDomain : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateDelegate( 
            /* [in] */ LPCWSTR wzAssemblyName,
            /* [in] */ LPCWSTR wzTypeName,
            /* [in] */ LPCWSTR wzMethodName,
            /* [out] */ LPVOID *ppvDelegate) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICLRPrivAppXDomainVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICLRPrivAppXDomain * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICLRPrivAppXDomain * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICLRPrivAppXDomain * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateDelegate )( 
            ICLRPrivAppXDomain * This,
            /* [in] */ LPCWSTR wzAssemblyName,
            /* [in] */ LPCWSTR wzTypeName,
            /* [in] */ LPCWSTR wzMethodName,
            /* [out] */ LPVOID *ppvDelegate);
        
        END_INTERFACE
    } ICLRPrivAppXDomainVtbl;

    interface ICLRPrivAppXDomain
    {
        CONST_VTBL struct ICLRPrivAppXDomainVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICLRPrivAppXDomain_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICLRPrivAppXDomain_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICLRPrivAppXDomain_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICLRPrivAppXDomain_CreateDelegate(This,wzAssemblyName,wzTypeName,wzMethodName,ppvDelegate)	\
    ( (This)->lpVtbl -> CreateDelegate(This,wzAssemblyName,wzTypeName,wzMethodName,ppvDelegate) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICLRPrivAppXDomain_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


