

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

#ifndef __xcordebug_h__
#define __xcordebug_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICorDebugMutableDataTarget_FWD_DEFINED__
#define __ICorDebugMutableDataTarget_FWD_DEFINED__
typedef interface ICorDebugMutableDataTarget ICorDebugMutableDataTarget;

#endif 	/* __ICorDebugMutableDataTarget_FWD_DEFINED__ */


#ifndef __ICorDebugProcess4_FWD_DEFINED__
#define __ICorDebugProcess4_FWD_DEFINED__
typedef interface ICorDebugProcess4 ICorDebugProcess4;

#endif 	/* __ICorDebugProcess4_FWD_DEFINED__ */


/* header files for imported files */
#include "cordebug.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_xcordebug_0000_0000 */
/* [local] */ 

typedef DWORD CORDB_CONTINUE_STATUS;



extern RPC_IF_HANDLE __MIDL_itf_xcordebug_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xcordebug_0000_0000_v0_0_s_ifspec;

#ifndef __ICorDebugMutableDataTarget_INTERFACE_DEFINED__
#define __ICorDebugMutableDataTarget_INTERFACE_DEFINED__

/* interface ICorDebugMutableDataTarget */
/* [unique][uuid][local][object] */ 


EXTERN_C const IID IID_ICorDebugMutableDataTarget;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A1B8A756-3CB6-4CCB-979F-3DF999673A59")
    ICorDebugMutableDataTarget : public ICorDebugDataTarget
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE WriteVirtual( 
            /* [in] */ CORDB_ADDRESS address,
            /* [size_is][in] */ const BYTE *pBuffer,
            /* [in] */ ULONG32 bytesRequested) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetThreadContext( 
            /* [in] */ DWORD dwThreadID,
            /* [in] */ ULONG32 contextSize,
            /* [size_is][in] */ const BYTE *pContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ContinueStatusChanged( 
            /* [in] */ DWORD dwThreadId,
            /* [in] */ CORDB_CONTINUE_STATUS continueStatus) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICorDebugMutableDataTargetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICorDebugMutableDataTarget * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICorDebugMutableDataTarget * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICorDebugMutableDataTarget * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPlatform )( 
            ICorDebugMutableDataTarget * This,
            /* [out] */ CorDebugPlatform *pTargetPlatform);
        
        HRESULT ( STDMETHODCALLTYPE *ReadVirtual )( 
            ICorDebugMutableDataTarget * This,
            /* [in] */ CORDB_ADDRESS address,
            /* [length_is][size_is][out] */ BYTE *pBuffer,
            /* [in] */ ULONG32 bytesRequested,
            /* [out] */ ULONG32 *pBytesRead);
        
        HRESULT ( STDMETHODCALLTYPE *GetThreadContext )( 
            ICorDebugMutableDataTarget * This,
            /* [in] */ DWORD dwThreadID,
            /* [in] */ ULONG32 contextFlags,
            /* [in] */ ULONG32 contextSize,
            /* [size_is][out] */ BYTE *pContext);
        
        HRESULT ( STDMETHODCALLTYPE *WriteVirtual )( 
            ICorDebugMutableDataTarget * This,
            /* [in] */ CORDB_ADDRESS address,
            /* [size_is][in] */ const BYTE *pBuffer,
            /* [in] */ ULONG32 bytesRequested);
        
        HRESULT ( STDMETHODCALLTYPE *SetThreadContext )( 
            ICorDebugMutableDataTarget * This,
            /* [in] */ DWORD dwThreadID,
            /* [in] */ ULONG32 contextSize,
            /* [size_is][in] */ const BYTE *pContext);
        
        HRESULT ( STDMETHODCALLTYPE *ContinueStatusChanged )( 
            ICorDebugMutableDataTarget * This,
            /* [in] */ DWORD dwThreadId,
            /* [in] */ CORDB_CONTINUE_STATUS continueStatus);
        
        END_INTERFACE
    } ICorDebugMutableDataTargetVtbl;

    interface ICorDebugMutableDataTarget
    {
        CONST_VTBL struct ICorDebugMutableDataTargetVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICorDebugMutableDataTarget_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICorDebugMutableDataTarget_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICorDebugMutableDataTarget_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICorDebugMutableDataTarget_GetPlatform(This,pTargetPlatform)	\
    ( (This)->lpVtbl -> GetPlatform(This,pTargetPlatform) ) 

#define ICorDebugMutableDataTarget_ReadVirtual(This,address,pBuffer,bytesRequested,pBytesRead)	\
    ( (This)->lpVtbl -> ReadVirtual(This,address,pBuffer,bytesRequested,pBytesRead) ) 

#define ICorDebugMutableDataTarget_GetThreadContext(This,dwThreadID,contextFlags,contextSize,pContext)	\
    ( (This)->lpVtbl -> GetThreadContext(This,dwThreadID,contextFlags,contextSize,pContext) ) 


#define ICorDebugMutableDataTarget_WriteVirtual(This,address,pBuffer,bytesRequested)	\
    ( (This)->lpVtbl -> WriteVirtual(This,address,pBuffer,bytesRequested) ) 

#define ICorDebugMutableDataTarget_SetThreadContext(This,dwThreadID,contextSize,pContext)	\
    ( (This)->lpVtbl -> SetThreadContext(This,dwThreadID,contextSize,pContext) ) 

#define ICorDebugMutableDataTarget_ContinueStatusChanged(This,dwThreadId,continueStatus)	\
    ( (This)->lpVtbl -> ContinueStatusChanged(This,dwThreadId,continueStatus) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICorDebugMutableDataTarget_INTERFACE_DEFINED__ */


#ifndef __ICorDebugProcess4_INTERFACE_DEFINED__
#define __ICorDebugProcess4_INTERFACE_DEFINED__

/* interface ICorDebugProcess4 */
/* [unique][uuid][local][object] */ 

typedef 
enum CorDebugFilterFlagsWindows
    {
        IS_FIRST_CHANCE	= 1
    } 	CorDebugFilterFlagsWindows;

typedef 
enum CorDebugRecordFormat
    {
        FORMAT_WINDOWS_EXCEPTIONRECORD32	= 1,
        FORMAT_WINDOWS_EXCEPTIONRECORD64	= 2
    } 	CorDebugRecordFormat;

typedef 
enum CorDebugStateChange
    {
        PROCESS_RUNNING	= 0x1,
        FLUSH_ALL	= 0x2
    } 	CorDebugStateChange;


EXTERN_C const IID IID_ICorDebugProcess4;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E930C679-78AF-4953-8AB7-B0AABF0F9F80")
    ICorDebugProcess4 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Filter( 
            /* [size_is][length_is][in] */ const BYTE pRecord[  ],
            /* [in] */ DWORD countBytes,
            /* [in] */ CorDebugRecordFormat format,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwThreadId,
            /* [in] */ ICorDebugManagedCallback *pCallback,
            /* [out][in] */ CORDB_CONTINUE_STATUS *pContinueStatus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ProcessStateChanged( 
            /* [in] */ CorDebugStateChange eChange) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICorDebugProcess4Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICorDebugProcess4 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICorDebugProcess4 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICorDebugProcess4 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Filter )( 
            ICorDebugProcess4 * This,
            /* [size_is][length_is][in] */ const BYTE pRecord[  ],
            /* [in] */ DWORD countBytes,
            /* [in] */ CorDebugRecordFormat format,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwThreadId,
            /* [in] */ ICorDebugManagedCallback *pCallback,
            /* [out][in] */ CORDB_CONTINUE_STATUS *pContinueStatus);
        
        HRESULT ( STDMETHODCALLTYPE *ProcessStateChanged )( 
            ICorDebugProcess4 * This,
            /* [in] */ CorDebugStateChange eChange);
        
        END_INTERFACE
    } ICorDebugProcess4Vtbl;

    interface ICorDebugProcess4
    {
        CONST_VTBL struct ICorDebugProcess4Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICorDebugProcess4_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICorDebugProcess4_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICorDebugProcess4_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICorDebugProcess4_Filter(This,pRecord,countBytes,format,dwFlags,dwThreadId,pCallback,pContinueStatus)	\
    ( (This)->lpVtbl -> Filter(This,pRecord,countBytes,format,dwFlags,dwThreadId,pCallback,pContinueStatus) ) 

#define ICorDebugProcess4_ProcessStateChanged(This,eChange)	\
    ( (This)->lpVtbl -> ProcessStateChanged(This,eChange) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICorDebugProcess4_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


