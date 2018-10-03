

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

#ifndef __binderngen_h__
#define __binderngen_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAssemblyLocation_FWD_DEFINED__
#define __IAssemblyLocation_FWD_DEFINED__
typedef interface IAssemblyLocation IAssemblyLocation;

#endif 	/* __IAssemblyLocation_FWD_DEFINED__ */


#ifndef __IBindResult_FWD_DEFINED__
#define __IBindResult_FWD_DEFINED__
typedef interface IBindResult IBindResult;

#endif 	/* __IBindResult_FWD_DEFINED__ */


#ifndef __IBindContext_FWD_DEFINED__
#define __IBindContext_FWD_DEFINED__
typedef interface IBindContext IBindContext;

#endif 	/* __IBindContext_FWD_DEFINED__ */


/* header files for imported files */
#include "objidl.h"
#include "fusion.h"
#include "fusionpriv.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_binderngen_0000_0000 */
/* [local] */ 


#ifdef _MSC_VER
#pragma comment(lib,"uuid.lib")
#endif

//---------------------------------------------------------------------------=
// Binder Interfaces
interface INativeImageInstallInfo;
interface IILFingerprint;
interface IILFingerprintFactory;
#ifdef _MSC_VER
#pragma once
#endif





typedef /* [public] */ 
enum __MIDL___MIDL_itf_binderngen_0000_0000_0001
    {
        PRE_BIND_NONE	= 0,
        PRE_BIND_APPLY_POLICY	= 0x80
    } 	PRE_BIND_FLAGS;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_binderngen_0000_0000_0002
    {
        ASSEMBLY_LOCATION_PATH	= 0x1,
        ASSEMBLY_LOCATION_GAC	= 0x2,
        ASSEMBLY_LOCATION_HAS	= 0x4,
        ASSEMBLY_LOCATION_BYTE_ARRAY	= 0x8
    } 	ASSEMBLY_LOCATION_FLAGS;

// {6970aad6-d5fb-469b-9098-a03ddc65a4bd}
EXTERN_GUID(IID_IAssemblyLocation, 0x6970aad6, 0xd5fb, 0x469b, 0x90, 0x98, 0xa0, 0x3d, 0xdc, 0x65, 0xa4, 0xbd);


extern RPC_IF_HANDLE __MIDL_itf_binderngen_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_binderngen_0000_0000_v0_0_s_ifspec;

#ifndef __IAssemblyLocation_INTERFACE_DEFINED__
#define __IAssemblyLocation_INTERFACE_DEFINED__

/* interface IAssemblyLocation */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IAssemblyLocation *LPASSEMBLYLOCATION;


EXTERN_C const IID IID_IAssemblyLocation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6970aad6-d5fb-469b-9098-a03ddc65a4bd")
    IAssemblyLocation : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetLocationType( 
            /* [in] */ DWORD *pdwLocationType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPath( 
            /* [annotation][in] */ 
            __inout_ecount(*pdwccAssemblyPath)  LPWSTR pwzAssemblyPath,
            /* [annotation][in] */ 
            __inout  LPDWORD pdwccAssemblyPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHostID( 
            UINT64 *puiHostID) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAssemblyLocationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAssemblyLocation * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAssemblyLocation * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAssemblyLocation * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocationType )( 
            IAssemblyLocation * This,
            /* [in] */ DWORD *pdwLocationType);
        
        HRESULT ( STDMETHODCALLTYPE *GetPath )( 
            IAssemblyLocation * This,
            /* [annotation][in] */ 
            __inout_ecount(*pdwccAssemblyPath)  LPWSTR pwzAssemblyPath,
            /* [annotation][in] */ 
            __inout  LPDWORD pdwccAssemblyPath);
        
        HRESULT ( STDMETHODCALLTYPE *GetHostID )( 
            IAssemblyLocation * This,
            UINT64 *puiHostID);
        
        END_INTERFACE
    } IAssemblyLocationVtbl;

    interface IAssemblyLocation
    {
        CONST_VTBL struct IAssemblyLocationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAssemblyLocation_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAssemblyLocation_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAssemblyLocation_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAssemblyLocation_GetLocationType(This,pdwLocationType)	\
    ( (This)->lpVtbl -> GetLocationType(This,pdwLocationType) ) 

#define IAssemblyLocation_GetPath(This,pwzAssemblyPath,pdwccAssemblyPath)	\
    ( (This)->lpVtbl -> GetPath(This,pwzAssemblyPath,pdwccAssemblyPath) ) 

#define IAssemblyLocation_GetHostID(This,puiHostID)	\
    ( (This)->lpVtbl -> GetHostID(This,puiHostID) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAssemblyLocation_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_binderngen_0000_0001 */
/* [local] */ 

// {82cff42f-55c2-45d2-aa6b-ff96e4de889c}
EXTERN_GUID(IID_IBindResult, 0x82cff42f, 0x55c2, 0x45d2, 0xaa, 0x6b, 0xff, 0x96, 0xe4, 0xde, 0x88, 0x9c);


extern RPC_IF_HANDLE __MIDL_itf_binderngen_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_binderngen_0000_0001_v0_0_s_ifspec;

#ifndef __IBindResult_INTERFACE_DEFINED__
#define __IBindResult_INTERFACE_DEFINED__

/* interface IBindResult */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IBindResult *LPBINDRESULT;


enum __MIDL_IBindResult_0001
    {
        IBindResultFlag_Default	= 0,
        IBindResultFlag_AssemblyNameDefIncomplete	= 0x1
    } ;

EXTERN_C const IID IID_IBindResult;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("82cff42f-55c2-45d2-aa6b-ff96e4de889c")
    IBindResult : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetAssemblyNameDef( 
            /* [out] */ IAssemblyName **ppIAssemblyNameDef) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextAssemblyModuleName( 
            /* [in] */ DWORD dwNIndex,
            /* [annotation][out][in] */ 
            __inout_ecount(*pdwCCModuleName)  LPWSTR pwzModuleName,
            /* [annotation][out][in] */ 
            __inout  LPDWORD pdwCCModuleName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAssemblyLocation( 
            /* [out] */ IAssemblyLocation **ppIAssemblyLocation) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNativeImage( 
            /* [out] */ IBindResult **ppINativeImage,
            /* [out] */ BOOL *pfNativeImageProbed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetNativeImage( 
            /* [in] */ IBindResult *pINativeImage,
            /* [out] */ IBindResult **ppINativeImageFinal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsEqual( 
            /* [in] */ IUnknown *pIUnk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextAssemblyNameRef( 
            /* [in] */ DWORD dwNIndex,
            /* [out] */ IAssemblyName **ppIAssemblyNameRef) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextDependentAssembly( 
            /* [in] */ DWORD dwNIndex,
            /* [out] */ IUnknown **ppIUnknownAssembly) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAssemblyLocationOfILImage( 
            /* [out] */ IAssemblyLocation **ppAssemblyLocation) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetILFingerprint( 
            /* [out] */ IILFingerprint **ppFingerprint) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSourceILTimestamp( 
            /* [out] */ FILETIME *pFileTime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNIInfo( 
            /* [out] */ INativeImageInstallInfo **pInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFlags( 
            DWORD *pdwFlags) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IBindResultVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBindResult * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBindResult * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBindResult * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetAssemblyNameDef )( 
            IBindResult * This,
            /* [out] */ IAssemblyName **ppIAssemblyNameDef);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextAssemblyModuleName )( 
            IBindResult * This,
            /* [in] */ DWORD dwNIndex,
            /* [annotation][out][in] */ 
            __inout_ecount(*pdwCCModuleName)  LPWSTR pwzModuleName,
            /* [annotation][out][in] */ 
            __inout  LPDWORD pdwCCModuleName);
        
        HRESULT ( STDMETHODCALLTYPE *GetAssemblyLocation )( 
            IBindResult * This,
            /* [out] */ IAssemblyLocation **ppIAssemblyLocation);
        
        HRESULT ( STDMETHODCALLTYPE *GetNativeImage )( 
            IBindResult * This,
            /* [out] */ IBindResult **ppINativeImage,
            /* [out] */ BOOL *pfNativeImageProbed);
        
        HRESULT ( STDMETHODCALLTYPE *SetNativeImage )( 
            IBindResult * This,
            /* [in] */ IBindResult *pINativeImage,
            /* [out] */ IBindResult **ppINativeImageFinal);
        
        HRESULT ( STDMETHODCALLTYPE *IsEqual )( 
            IBindResult * This,
            /* [in] */ IUnknown *pIUnk);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextAssemblyNameRef )( 
            IBindResult * This,
            /* [in] */ DWORD dwNIndex,
            /* [out] */ IAssemblyName **ppIAssemblyNameRef);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextDependentAssembly )( 
            IBindResult * This,
            /* [in] */ DWORD dwNIndex,
            /* [out] */ IUnknown **ppIUnknownAssembly);
        
        HRESULT ( STDMETHODCALLTYPE *GetAssemblyLocationOfILImage )( 
            IBindResult * This,
            /* [out] */ IAssemblyLocation **ppAssemblyLocation);
        
        HRESULT ( STDMETHODCALLTYPE *GetILFingerprint )( 
            IBindResult * This,
            /* [out] */ IILFingerprint **ppFingerprint);
        
        HRESULT ( STDMETHODCALLTYPE *GetSourceILTimestamp )( 
            IBindResult * This,
            /* [out] */ FILETIME *pFileTime);
        
        HRESULT ( STDMETHODCALLTYPE *GetNIInfo )( 
            IBindResult * This,
            /* [out] */ INativeImageInstallInfo **pInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetFlags )( 
            IBindResult * This,
            DWORD *pdwFlags);
        
        END_INTERFACE
    } IBindResultVtbl;

    interface IBindResult
    {
        CONST_VTBL struct IBindResultVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBindResult_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBindResult_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBindResult_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBindResult_GetAssemblyNameDef(This,ppIAssemblyNameDef)	\
    ( (This)->lpVtbl -> GetAssemblyNameDef(This,ppIAssemblyNameDef) ) 

#define IBindResult_GetNextAssemblyModuleName(This,dwNIndex,pwzModuleName,pdwCCModuleName)	\
    ( (This)->lpVtbl -> GetNextAssemblyModuleName(This,dwNIndex,pwzModuleName,pdwCCModuleName) ) 

#define IBindResult_GetAssemblyLocation(This,ppIAssemblyLocation)	\
    ( (This)->lpVtbl -> GetAssemblyLocation(This,ppIAssemblyLocation) ) 

#define IBindResult_GetNativeImage(This,ppINativeImage,pfNativeImageProbed)	\
    ( (This)->lpVtbl -> GetNativeImage(This,ppINativeImage,pfNativeImageProbed) ) 

#define IBindResult_SetNativeImage(This,pINativeImage,ppINativeImageFinal)	\
    ( (This)->lpVtbl -> SetNativeImage(This,pINativeImage,ppINativeImageFinal) ) 

#define IBindResult_IsEqual(This,pIUnk)	\
    ( (This)->lpVtbl -> IsEqual(This,pIUnk) ) 

#define IBindResult_GetNextAssemblyNameRef(This,dwNIndex,ppIAssemblyNameRef)	\
    ( (This)->lpVtbl -> GetNextAssemblyNameRef(This,dwNIndex,ppIAssemblyNameRef) ) 

#define IBindResult_GetNextDependentAssembly(This,dwNIndex,ppIUnknownAssembly)	\
    ( (This)->lpVtbl -> GetNextDependentAssembly(This,dwNIndex,ppIUnknownAssembly) ) 

#define IBindResult_GetAssemblyLocationOfILImage(This,ppAssemblyLocation)	\
    ( (This)->lpVtbl -> GetAssemblyLocationOfILImage(This,ppAssemblyLocation) ) 

#define IBindResult_GetILFingerprint(This,ppFingerprint)	\
    ( (This)->lpVtbl -> GetILFingerprint(This,ppFingerprint) ) 

#define IBindResult_GetSourceILTimestamp(This,pFileTime)	\
    ( (This)->lpVtbl -> GetSourceILTimestamp(This,pFileTime) ) 

#define IBindResult_GetNIInfo(This,pInfo)	\
    ( (This)->lpVtbl -> GetNIInfo(This,pInfo) ) 

#define IBindResult_GetFlags(This,pdwFlags)	\
    ( (This)->lpVtbl -> GetFlags(This,pdwFlags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBindResult_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_binderngen_0000_0002 */
/* [local] */ 

// {88db0d1a-460e-4341-96d7-a7c9b68f4679}
EXTERN_GUID(IID_IBindContext, 0x88db0d1a, 0x460e, 0x4341, 0x96, 0xd7, 0xa7, 0xc9, 0xb6, 0x8f, 0x46, 0x79);


extern RPC_IF_HANDLE __MIDL_itf_binderngen_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_binderngen_0000_0002_v0_0_s_ifspec;

#ifndef __IBindContext_INTERFACE_DEFINED__
#define __IBindContext_INTERFACE_DEFINED__

/* interface IBindContext */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IBindContext *LPBINDCONTEXT;


EXTERN_C const IID IID_IBindContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("88db0d1a-460e-4341-96d7-a7c9b68f4679")
    IBindContext : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE PreBind( 
            /* [in] */ IAssemblyName *pIAssemblyName,
            /* [in] */ DWORD dwPreBindFlags,
            /* [out] */ IBindResult **ppIBindResult) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsDefaultContext( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IBindContextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBindContext * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBindContext * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBindContext * This);
        
        HRESULT ( STDMETHODCALLTYPE *PreBind )( 
            IBindContext * This,
            /* [in] */ IAssemblyName *pIAssemblyName,
            /* [in] */ DWORD dwPreBindFlags,
            /* [out] */ IBindResult **ppIBindResult);
        
        HRESULT ( STDMETHODCALLTYPE *IsDefaultContext )( 
            IBindContext * This);
        
        END_INTERFACE
    } IBindContextVtbl;

    interface IBindContext
    {
        CONST_VTBL struct IBindContextVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBindContext_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBindContext_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBindContext_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBindContext_PreBind(This,pIAssemblyName,dwPreBindFlags,ppIBindResult)	\
    ( (This)->lpVtbl -> PreBind(This,pIAssemblyName,dwPreBindFlags,ppIBindResult) ) 

#define IBindContext_IsDefaultContext(This)	\
    ( (This)->lpVtbl -> IsDefaultContext(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBindContext_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_binderngen_0000_0003 */
/* [local] */ 

STDAPI GetBindResultFromPath(LPCWSTR pwzFilePath, DWORD dwMetaDataImportFlags, DWORD dwAssemblyLocation, IBindContext *pBindContext, IBindResult **ppIBindResult, HANDLE hFile = INVALID_HANDLE_VALUE); 
typedef /* [public] */ struct __MIDL___MIDL_itf_binderngen_0000_0003_0001
    {
    WCHAR *pwzZapConfigString;
    DWORD dwZapConfigMask;
    } 	NativeConfigData;

interface ICorSvcLogger;
STDAPI InitializeNativeBinder(); 
STDAPI GetZapDir(__deref_out LPCWSTR *pszZapDir); 
STDAPI InstallNativeAssembly(LPCWSTR pwzAssemblyPath, HANDLE hFile, LPCWSTR pwzZapSet, IBindContext* pBindCtx, IAssemblyName **ppAssemblyName, IAssemblyLocation **ppAssemblyLocation);
STDAPI UninstallNativeAssembly(IAssemblyName *pIAssemblyName, ICorSvcLogger *pLogSink); 
STDAPI QueryNativeAssemblyInfo(IAssemblyName *pIAssemblyName, __out_ecount(*pdwSize) LPWSTR pwzFilePath, __inout LPDWORD pdwSize); 
STDAPI GetAssemblyNameFromNIPath(LPCWSTR wszNIPath, IAssemblyName ** ppAssemblyName); 
STDAPI CreateNativeAssemblyEnum(IAssemblyEnum **ppIAssemblyEnum, IAssemblyName  *pIAssemblyName);
STDAPI BindToNativeAssembly(IBindResult *pGivenILAsm, const NativeConfigData *pNativeConfigData, IBindContext *pBindContext, IFusionBindLog *pFusionBindLog); 
struct CORCOMPILE_TARGETEDPATCHBAND;
struct IMDInternalImport;
STDAPI ReadTargetedPatchBandFromMetaData(struct IMDInternalImport *pInternalImport, /* [out] */ struct CORCOMPILE_TARGETEDPATCHBAND *pTargetedPatchBandOut); 


extern RPC_IF_HANDLE __MIDL_itf_binderngen_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_binderngen_0000_0003_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


