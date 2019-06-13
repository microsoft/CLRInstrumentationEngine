// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "unknwn.h"
#include <pthread.h>

// --- mstypes.h ---
#define NEAR
#define FAR
typedef LONG LSTATUS;
#define _T(x) u ## x

#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#define _VARIANT_BOOL    /##/
typedef ULONG PROPID;
typedef struct tagCLIPDATA
{
    ULONG cbSize;
    long ulClipFmt;
    /* [size_is] */ BYTE *pClipData;
} CLIPDATA;
typedef struct tagBSTRBLOB
{
    ULONG cbSize;
    /* [size_is] */ BYTE *pData;
} BSTRBLOB;

typedef WCHAR* STRSAFE_LPWSTR;
typedef const WCHAR* STRSAFE_LPCWSTR;
typedef CHAR* STRSAFE_LPSTR;
typedef const CHAR* STRSAFE_LPCSTR;
#define Int32x32To64(a, b)  (((__int64)((long)(a))) * ((__int64)((long)(b))))
#define UInt32x32To64(a, b) (((unsigned __int64)((unsigned int)(a))) * ((unsigned __int64)((unsigned int)(b))))

inline void * _recalloc(void *memblock, size_t num, size_t size)
{
    return realloc(memblock, num * size);
}
#define _InterlockedCompareExchange InterlockedCompareExchange

#define MAXUINT     ((UINT)~((UINT)0))
#define LLONG_MAX     9223372036854775807       /* maximum signed long long int value */
#define LLONG_MIN   (-9223372036854775807 - 1)  /* minimum signed long long int value */
#define ULLONG_MAX    0xffffffffffffffffu       /* maximum unsigned long long int value */
EXTERN_C PALAPI errno_t _ultow_s( unsigned long inValue, WCHAR* outBuffer, size_t inDestBufferSize, int inRadix );

inline LONG _InterlockedIncrement(LONG volatile * _Addend)
{
    return InterlockedIncrement(_Addend);
}

inline LONG _InterlockedDecrement(LONG volatile * _Addend)
{
    return InterlockedDecrement(_Addend);
}

inline LONG _InterlockedOr(LONG volatile * Destination, LONG Value)
{
    return InterlockedOr(Destination, Value);
}

inline LONG _InterlockedAnd(LONG volatile * Destination, LONG Value)
{
    return InterlockedAnd(Destination, Value);
}

inline LONG _InterlockedExchange(LONG volatile * Target, LONG Value)
{
    return InterlockedExchange(Target, Value);
}

// --- guid ---
__inline int InlineIsEqualGUID(REFGUID rguid1, REFGUID rguid2)
{
   return (
      ((ULONG *) &rguid1)[0] == ((ULONG *) &rguid2)[0] &&
      ((ULONG *) &rguid1)[1] == ((ULONG *) &rguid2)[1] &&
      ((ULONG *) &rguid1)[2] == ((ULONG *) &rguid2)[2] &&
      ((ULONG *) &rguid1)[3] == ((ULONG *) &rguid2)[3]);
}


// --- winerror ---
#define ERROR_INVALID_THREAD_ID          1444L
#define ERROR_ELEVATION_REQUIRED         740L
#define ERROR_ALREADY_INITIALIZED        1247L
#define ERROR_BAD_ARGUMENTS              160L
#define RPC_E_CALL_REJECTED              _HRESULT_TYPEDEF_(0x80010001L)
#define RPC_E_NO_SYNC                    _HRESULT_TYPEDEF_(0x80010120L)
#define RPC_S_CALLPENDING                _HRESULT_TYPEDEF_(0x80010115L)
#define DISP_E_ARRAYISLOCKED             _HRESULT_TYPEDEF_(0x8002000DL)
#define RPC_E_WRONG_THREAD               _HRESULT_TYPEDEF_(0x8001010EL)
#define E_ILLEGAL_METHOD_CALL            _HRESULT_TYPEDEF_(0x8000000EL)
#define E_NOT_VALID_STATE                _HRESULT_TYPEDEF_(0x8007139FL)


// -- string --
typedef BSTR *LPBSTR;
#define MAKEINTRESOURCE     MAKEINTRESOURCEW
#define MAKEINTRESOURCEW(i) ((LPWSTR)((ULONG_PTR)((WORD)(i))))
#define RT_STRING           MAKEINTRESOURCEW(6)
#define LOAD_LIBRARY_SEARCH_SYSTEM32        0x00000800
#define IS_INTRESOURCE(_r) ((((ULONG_PTR)(_r)) >> 16) == 0)

typedef struct threadmbcinfostruct {
        int refcount;
        int mbcodepage;
        int ismbcodepage;
        unsigned short mbulinfo[6];
        unsigned char mbctype[257];
        unsigned char mbcasemap[256];
        const WCHAR* mblocalename;
} threadmbcinfo;

typedef struct threadlocaleinfostruct * pthreadlocinfo;
typedef struct threadmbcinfostruct * pthreadmbcinfo;
typedef struct localeinfo_struct
{
    pthreadlocinfo locinfo;
    pthreadmbcinfo mbcinfo;
} _locale_tstruct, *_locale_t;


EXTERN_C PALAPI size_t PAL_wcsnlen (const WCHAR*__s, size_t __maxlen);
EXTERN_C PALAPI int _vwprintf (const WCHAR *format, va_list arg);
EXTERN_C PALAPI PVOID EncodePointer(PVOID Ptr);
EXTERN_C PALAPI PVOID DecodePointer(PVOID Ptr);
EXTERN_C PALAPI LPWSTR CharLowerW(LPWSTR lpsz);
EXTERN_C PALAPI LPWSTR CharUpperW(LPWSTR lpsz);
EXTERN_C PALAPI DWORD CharLowerBuffW(LPWSTR lpsz, DWORD cchLength);
EXTERN_C PALAPI DWORD CharUpperBuffW(LPWSTR lpsz, DWORD cchLength);
EXTERN_C PALAPI int wvsprintfW(LPWSTR, LPCWSTR, va_list arglist);

EXTERN_C LCID GetThreadLocale();

EXTERN_C PALAPI WCHAR *PAL_wmemcpy(WCHAR *dest, const WCHAR *src, size_t count);

inline WCHAR *_wcsdup(const WCHAR *strSource)
{
    int len = PAL_wcslen(strSource);
    WCHAR *tgt = (WCHAR *)malloc((len + 1) * sizeof (WCHAR));
    wcsncpy_s(tgt, len + 1, strSource, len);
    return tgt;
}

//--- UNKNWN.H ---
#include "oaidl.h"
#define CY_HI(x) (x).u.Hi
#define CY_LO(x) (x).u.Lo
MIDL_INTERFACE("FC4801A3-2BA9-11CF-A229-00AA003D7352")
IObjectWithSite : public IUnknown
{
public:
    virtual HRESULT SetSite(
        /* [in] */ IUnknown *pUnkSite) = 0;

    virtual HRESULT GetSite(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void **ppvSite) = 0;

};

MIDL_INTERFACE("00000101-0000-0000-C000-000000000046")
IEnumString : public IUnknown
{
public:
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Next(
        /* [in] */ ULONG celt,
        /* [annotation] */
        _Out_writes_to_(celt,*pceltFetched)  LPOLESTR *rgelt,
        /* [annotation] */
        _Out_opt_  ULONG *pceltFetched) = 0;

    virtual HRESULT STDMETHODCALLTYPE Skip(
        /* [in] */ ULONG celt) = 0;

    virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;

    virtual HRESULT STDMETHODCALLTYPE Clone(
        /* [out] */ __RPC__deref_out_opt IEnumString **ppenum) = 0;

};

// --- oaidl.h ---

#define	DISPID_PROPERTYPUT	( -3 )
typedef LONG DISPID;
typedef struct tagDISPPARAMS
{
    /* [size_is] */ VARIANTARG *rgvarg;
    /* [size_is] */ DISPID *rgdispidNamedArgs;
    UINT cArgs;
    UINT cNamedArgs;
} DISPPARAMS;

MIDL_INTERFACE("00020400-0000-0000-C000-000000000046")
IDispatch : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
        /* [out] */ __RPC__out UINT *pctinfo) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(
        /* [in] */ UINT iTInfo,
        /* [in] */ LCID lcid,
        /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(
        /* [in] */ REFIID riid,
        /* [size_is][in] */ LPOLESTR *rgszNames,
        /* [range][in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [size_is][out] */ DISPID *rgDispId) = 0;

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
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
        _Out_opt_  UINT *puArgErr) = 0;
};

typedef IUnknown *LPUNKNOWN;
// So we can have CoCreateInstance in most of the code base,
// instead of spreading around of if'def FEATURE_PALs for PAL_CoCreateInstance.
// #define CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv) Unix_CoCreateInstance(rclsid, riid, ppv)
EXTERN_C HRESULT CoCreateInstance(REFCLSID   rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext,
                             REFIID     riid, void     **ppv);
// This is defined in palrt.h.
// TODO: Fix palrt.h
#undef CoCreateInstance

// -- winnt.h ---
typedef LONG DISPID;
#define MAKELANGID(p, s)       ((((WORD  )(s)) << 10) | (WORD  )(p))
#define HEAP_GENERATE_EXCEPTIONS        0x00000004
#define FILE_WRITE_DATA                  (0x0002)

// --- winerror.h ---
// TODO: Move to pal_error.h
#define RPC_S_SERVER_UNAVAILABLE         1722L
#define RPC_S_CALL_FAILED_DNE            1727L
#define RPC_S_CALL_FAILED                1726L
#define RPC_S_UNKNOWN_IF                 1717L
#define RPC_E_DISCONNECTED               _HRESULT_TYPEDEF_(0x80010108L)
#define RPC_E_SERVER_DIED                _HRESULT_TYPEDEF_(0x80010007L)
#define RPC_E_CLIENT_DIED                _HRESULT_TYPEDEF_(0x80010008L)
#define RPC_E_CALL_CANCELED              _HRESULT_TYPEDEF_(0x80010002L)
#define RPC_E_SERVERFAULT                _HRESULT_TYPEDEF_(0x80010105L)
#define LOCALE_USER_DEFAULT    (MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT))
#define LANG_USER_DEFAULT      (MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT))
#define LANG_NEUTRAL                     0x00
#define LANG_INVARIANT                   0x7f
#define SUBLANG_NEUTRAL                  0x00    // language neutral
#define SUBLANG_DEFAULT                             0x01    // user default
#define SUBLANG_SYS_DEFAULT                         0x02    // system default
#define LOCALE_USER_DEFAULT    (MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT))
#define MAKELANGID(p, s)       ((((WORD  )(s)) << 10) | (WORD  )(p))
#define SORT_DEFAULT                     0x0     // sorting default
#define MAKELCID(lgid, srtid)  ((DWORD)((((DWORD)((WORD  )(srtid))) << 16) |  \
                                         ((DWORD)((WORD  )(lgid)))))
#define LANGIDFROMLCID(lcid)   ((WORD  )(lcid))



// --- winuser.h ---

#define SW_HIDE             0
#define WM_USER                         0x0400
#define WM_QUIT                         0x0012
#define PM_NOREMOVE         0x0000

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT;

/*
 * Message structure
 */
typedef struct tagMSG {
    HWND        hwnd;
    UINT        message;
    WPARAM      wParam;
    LPARAM      lParam;
    DWORD       time;
    POINT       pt;
#ifdef _MAC
    DWORD       lPrivate;
#endif
} MSG, *PMSG, *LPMSG;

// --- windef.h ---
typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT, *PRECT, NEAR *NPRECT, FAR *LPRECT;


// --- com ---
#define FADF_VARIANT     ( 0x800 )
#define FADF_BSTR        ( 0x100 )
#define FADF_HAVEVARTYPE ( 0x80 )
#define FADF_SINGLEALLOC ( 0x2000 )

typedef enum tagCOINIT
{
  COINIT_APARTMENTTHREADED  = 0x2,      // Apartment model

  // These constants are only valid on Windows NT 4.0
  COINIT_MULTITHREADED      = 0x0,
  COINIT_DISABLE_OLE1DDE    = 0x4,      // Don't use DDE for Ole1 support.
  COINIT_SPEED_OVER_MEMORY  = 0x8,      // Trade memory for speed.
} COINIT;

typedef
enum tagCLSCTX
    {
        CLSCTX_INPROC_SERVER	= 0x1,
        CLSCTX_INPROC_HANDLER	= 0x2,
        CLSCTX_LOCAL_SERVER	= 0x4,
        CLSCTX_INPROC_SERVER16	= 0x8,
        CLSCTX_REMOTE_SERVER	= 0x10,
        CLSCTX_INPROC_HANDLER16	= 0x20,
        CLSCTX_RESERVED1	= 0x40,
        CLSCTX_RESERVED2	= 0x80,
        CLSCTX_RESERVED3	= 0x100,
        CLSCTX_RESERVED4	= 0x200,
        CLSCTX_NO_CODE_DOWNLOAD	= 0x400,
        CLSCTX_RESERVED5	= 0x800,
        CLSCTX_NO_CUSTOM_MARSHAL	= 0x1000,
        CLSCTX_ENABLE_CODE_DOWNLOAD	= 0x2000,
        CLSCTX_NO_FAILURE_LOG	= 0x4000,
        CLSCTX_DISABLE_AAA	= 0x8000,
        CLSCTX_ENABLE_AAA	= 0x10000,
        CLSCTX_FROM_DEFAULT_CONTEXT	= 0x20000,
        CLSCTX_ACTIVATE_32_BIT_SERVER	= 0x40000,
        CLSCTX_ACTIVATE_64_BIT_SERVER	= 0x80000,
        CLSCTX_ENABLE_CLOAKING	= 0x100000,
        CLSCTX_APPCONTAINER	= 0x400000,
        CLSCTX_ACTIVATE_AAA_AS_IU	= 0x800000,
        CLSCTX_PS_DLL	= ( int  )0x80000000
    } 	CLSCTX;
#define CLSCTX_INPROC           (CLSCTX_INPROC_SERVER|CLSCTX_INPROC_HANDLER)
#define CLSCTX_ALL              (CLSCTX_INPROC_SERVER| \
                                 CLSCTX_INPROC_HANDLER| \
                                 CLSCTX_LOCAL_SERVER| \
                                 CLSCTX_REMOTE_SERVER)

#define CLSCTX_SERVER           (CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER|CLSCTX_REMOTE_SERVER)

#ifdef PLATFORM_UNIX
// Use this instead of the fake method that was here in debugger's version of palrt2.h.
#define CLSIDFromString IIDFromString
#endif

// --- ui ---
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
#define MB_ICONHAND                 0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONASTERISK             0x00000040L
#define MB_DEFBUTTON1               0x00000000L
#define MB_DEFBUTTON2               0x00000100L
#define MB_DEFBUTTON3               0x00000200L
#define MB_USERICON                 0x00000080L
#define MB_ICONWARNING              MB_ICONEXCLAMATION
#define MB_ICONERROR                MB_ICONHAND

// --- winbase.h ---
#define CREATE_UNICODE_ENVIRONMENT        0x00000400

// --- misc ---
#define DELETE                           (0x00010000L)
#define READ_CONTROL                     (0x00020000L)
#define WRITE_DAC                        (0x00040000L)
#define WRITE_OWNER                      (0x00080000L)
#define SYNCHRONIZE                      (0x00100000L)

#define STANDARD_RIGHTS_REQUIRED         (0x000F0000L)

#define STANDARD_RIGHTS_READ             (READ_CONTROL)
#define STANDARD_RIGHTS_WRITE            (READ_CONTROL)
#define STANDARD_RIGHTS_EXECUTE          (READ_CONTROL)

#define STANDARD_RIGHTS_ALL              (0x001F0000L)

#define SPECIFIC_RIGHTS_ALL              (0x0000FFFFL)
#define PROCESS_ALL_ACCESS               (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0xFFFF)
#define GMEM_FIXED          0x0000
#define GMEM_ZEROINT        0x0040


typedef struct _TIME_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[ 32 ];
    SYSTEMTIME StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[ 32 ];
    SYSTEMTIME DaylightDate;
    LONG DaylightBias;
} TIME_ZONE_INFORMATION, *PTIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

typedef DWORD ACCESS_MASK;
typedef ACCESS_MASK REGSAM;
typedef HKEY *PHKEY;

EXTERN_C PALAPI size_t  __cdecl _msize(_Pre_notnull_ void * _Memory);
EXTERN_C PALAPI HLOCAL LocalReAlloc(HLOCAL hMem, SIZE_T uBytes, UINT uFlags);
EXTERN_C PALAPI SIZE_T LocalSize(HLOCAL hMem);
EXTERN_C PALAPI SIZE_T GlobalSize(HGLOBAL hMem);
EXTERN_C PALAPI LSTATUS RegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
EXTERN_C PALAPI LSTATUS RegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
EXTERN_C PALAPI LSTATUS RegCloseKey(HKEY hKey);
EXTERN_C PALAPI HGLOBAL GlobalAlloc(UINT uFlags, SIZE_T uBytes);
EXTERN_C PALAPI HGLOBAL GlobalReAlloc(HLOCAL hMem, SIZE_T uBytes, UINT uFlags);
EXTERN_C PALAPI HGLOBAL GlobalFree(HGLOBAL hMem);
EXTERN_C PALAPI LPVOID GlobalLock(HGLOBAL hMem);
EXTERN_C PALAPI BOOL GlobalUnlock(HGLOBAL hMem);
EXTERN_C PALAPI HRESULT CreateStreamOnHGlobalWithSize(PVOID hGlobal, SIZE_T size, BOOL fDeleteOnRelease, interface IStream** ppstm);
PALAPI DWORD ExpandEnvironmentStringsW(LPCWSTR lpSrc, LPWSTR lpDst, DWORD nSize);

#define SUBLANG_ENGLISH_US               0x01
#define RegDeleteKey  RegDeleteKeyW
#define LoadString LoadStringW
#define FindResourceEx FindResourceExW
#define	FADF_HAVEIID	( 0x40 )
#define	FADF_DISPATCH	( 0x400 )

#define WT_EXECUTEDEFAULT           0x00000000
#define WT_EXECUTELONGFUNCTION      0x00000010
#define WT_EXECUTEONLYONCE          0x00000008
#define PathFileExists  PathFileExistsW

PALAPI LSTATUS RegDeleteKeyW (HKEY hKey, LPCWSTR lpSubKey);
STDAPI_(LPVOID) CoTaskMemAlloc(SIZE_T cb);
STDAPI_(LPVOID) CoTaskMemRealloc(LPVOID pv, SIZE_T cb);
STDAPI_(void) CoTaskMemFree(LPVOID pv);
EXTERN_C PALAPI HGLOBAL LoadResource(HMODULE hModule, HRSRC hResInfo);
EXTERN_C PALAPI LPVOID LockResource(HGLOBAL hResData);
EXTERN_C PALAPI DWORD SizeofResource(HMODULE hModule, HRSRC hResInfo);
EXTERN_C PALAPI HRSRC FindResourceW(HMODULE hModule, LPCWSTR lpName, LPCWSTR lpType);
EXTERN_C PALAPI HRSRC FindResourceExW(HMODULE hModule, LPCWSTR lpType, LPCWSTR lpName, WORD wLanguage);
EXTERN_C PALAPI bool LoadStringW(HINSTANCE hInst, UINT nID, LPWSTR lpBuffer, int cchBufferMax);
EXTERN_C PALAPI BOOL SetDefaultDllDirectories(DWORD DirectoryFlags);
#ifdef _WINDOWS_
EXTERN_C PALAPI HMODULE GetModuleHandleW(LPCWSTR lpModuleName);
#endif
EXTERN_C PALAPI UINT GetSystemDirectoryW(LPWSTR lpBuffer, UINT uSize);
EXTERN_C PALAPI BOOL QueueUserWorkItem(LPTHREAD_START_ROUTINE Function, PVOID Context, ULONG Flags);
EXTERN_C PALAPI BOOL PathFileExistsW(LPCWSTR pszPath);
EXTERN_C PALAPI LPCWSTR PathFindExtensionW(LPCWSTR pszPath);
EXTERN_C PALAPI LPWSTR PAL_PathFindFileNameW(LPCWSTR pPath);
#define PathFindFileNameW PAL_PathFindFileNameW
EXTERN_C PALAPI BOOL EnumProcesses(DWORD * lpidProcess, DWORD cb, LPDWORD lpcbNeeded);
EXTERN_C PALAPI BOOL EnumProcessModules(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded);
EXTERN_C PALAPI BOOL UnregisterWaitEx(HANDLE WaitHandle, HANDLE CompletionEvent);
EXTERN_C PALAPI SIZE_T VirtualQueryEx(HANDLE hProcess, LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
EXTERN_C PALAPI BOOL VirtualFreeEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);

inline BOOL HeapDestroy(HANDLE hHeap)
{
    return true;
}

// -- others TODO: remove --
#define LOCKFILE_EXCLUSIVE_LOCK 0x00000002

typedef struct _IMAGE_ARM_RUNTIME_FUNCTION_ENTRY {
    DWORD BeginAddress;
    union {
        DWORD UnwindData;
        struct {
            DWORD Flag : 2;
            DWORD FunctionLength : 11;
            DWORD Ret : 2;
            DWORD H : 1;
            DWORD Reg : 3;
            DWORD R : 1;
            DWORD L : 1;
            DWORD C : 1;
            DWORD StackAdjust : 10;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} IMAGE_ARM_RUNTIME_FUNCTION_ENTRY, * PIMAGE_ARM_RUNTIME_FUNCTION_ENTRY;

typedef struct FLOAT10
{
    char b[10];
} FLOAT10;

#define TLS_MINIMUM_AVAILABLE     64
#define TLS_OUT_OF_INDEXES        ((DWORD)0xFFFFFFFF)
#define TOKEN_QUERY             (0x0008)

/*
 * Exception disposition return values.
 */
typedef enum _EXCEPTION_DISPOSITION {
    ExceptionContinueExecution,
    ExceptionContinueSearch,
    ExceptionNestedException,
    ExceptionCollidedUnwind
} EXCEPTION_DISPOSITION;

typedef
EXCEPTION_DISPOSITION
EXCEPTION_ROUTINE (
    _Inout_ struct _EXCEPTION_RECORD *ExceptionRecord,
    _In_ PVOID EstablisherFrame,
    _Inout_ struct _CONTEXT *ContextRecord,
    _In_ PVOID DispatcherContext
    );

typedef EXCEPTION_ROUTINE *PEXCEPTION_ROUTINE;

typedef struct _EXCEPTION_REGISTRATION_RECORD {
    struct _EXCEPTION_REGISTRATION_RECORD *Next;
    PEXCEPTION_ROUTINE Handler;
} EXCEPTION_REGISTRATION_RECORD;

typedef EXCEPTION_REGISTRATION_RECORD *PEXCEPTION_REGISTRATION_RECORD;

typedef struct _NT_TIB {
    struct _EXCEPTION_REGISTRATION_RECORD *ExceptionList;
    PVOID StackBase;
    PVOID StackLimit;
    PVOID SubSystemTib;
#if defined(_MSC_EXTENSIONS)
    union {
        PVOID FiberData;
        DWORD Version;
    };
#else
    PVOID FiberData;
#endif
    PVOID ArbitraryUserPointer;
    struct _NT_TIB *Self;
} NT_TIB;
typedef NT_TIB *PNT_TIB;

typedef struct _SID_AND_ATTRIBUTES {
#ifdef MIDL_PASS
    PISID Sid;
#else // MIDL_PASS
    PSID Sid;
#endif // MIDL_PASS
    DWORD Attributes;
} SID_AND_ATTRIBUTES, * PSID_AND_ATTRIBUTES;

typedef struct _TOKEN_USER {
    SID_AND_ATTRIBUTES User;
} TOKEN_USER, *PTOKEN_USER;

typedef struct _TOKEN_MANDATORY_LABEL {
    SID_AND_ATTRIBUTES Label;
} TOKEN_MANDATORY_LABEL, *PTOKEN_MANDATORY_LABEL;

typedef enum _SID_NAME_USE {
    SidTypeUser = 1,
    SidTypeGroup,
    SidTypeDomain,
    SidTypeAlias,
    SidTypeWellKnownGroup,
    SidTypeDeletedAccount,
    SidTypeInvalid,
    SidTypeUnknown,
    SidTypeComputer,
    SidTypeLabel
} SID_NAME_USE, *PSID_NAME_USE;

inline HANDLE FindFirstFileEx(LPCTSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData,
    FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags )
{
    if (fInfoLevelId != FindExInfoStandard || fSearchOp != FindExSearchNameMatch ||
        lpSearchFilter != nullptr || dwAdditionalFlags != 0)
    {
        return NULL;
    }
    return FindFirstFileW(lpFileName, static_cast<LPWIN32_FIND_DATAW>(lpFindFileData));
}

// Critical section object that has the same layout as the windows version
// PAL's critical section object size varies by platform due to changing size of pthread_mutex_t type it uses
// This causes us to have different versions of managed dispatcher per platform as it shares critical section objects with native code
// We override its definition and the Enter/Leave methods here and use a critical section that is of the same size
typedef struct _PAL_CRITICAL_SECTION {
    PVOID DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    ULONG_PTR OwningThread;
    pthread_mutex_t* Mutex;           // Corresponds to LockSemaphore member in windows
    ULONG_PTR SpinCount;
} PAL_CRITICAL_SECTION;

#undef CRITICAL_SECTION
#define CRITICAL_SECTION PAL_CRITICAL_SECTION

#undef PCRITICAL_SECTION
#define PCRITICAL_SECTION PAL_CRITICAL_SECTION*

#undef LPCRITICAL_SECTION
#define LPCRITICAL_SECTION PCRITICAL_SECTION

STDAPI_(void) PAL_EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
#define EnterCriticalSection PAL_EnterCriticalSection

STDAPI_(void) PAL_LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
#define LeaveCriticalSection PAL_LeaveCriticalSection

STDAPI_(void) PAL_InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
#define InitializeCriticalSection PAL_InitializeCriticalSection

STDAPI_(BOOL) PAL_InitializeCriticalSectionEx(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount, DWORD Flags);
#define InitializeCriticalSectionEx PAL_InitializeCriticalSectionEx

STDAPI_(void) PAL_DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
#define DeleteCriticalSection PAL_DeleteCriticalSection

STDAPI_(BOOL) PAL_TryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
#define TryEnterCriticalSection PAL_TryEnterCriticalSection

extern "C" BOOL __stdcall PathFileExistsW(LPCWSTR pszPath);
extern "C" LPCWSTR __stdcall PathFindExtensionW(LPCWSTR pszPath);
extern "C" LPWSTR PAL_PathFindFileNameW(LPCWSTR pPath);
extern "C" HRESULT PathCchAppend(
    _Inout_  WCHAR*  wszPath,
    _In_     size_t cchPath,
    _In_opt_ WCHAR* wszMore
);
extern "C"  HRESULT PathCchRemoveFileSpec(
    _Inout_ WCHAR*  wszPath,
    _In_    size_t cchPath
);

// -- locale definitions that were removed from PAL
#define MAKELCID(lgid, srtid)  ((DWORD)((((DWORD)((WORD  )(srtid))) << 16) |  \
                                         ((DWORD)((WORD  )(lgid)))))
#define LANGIDFROMLCID(lcid)   ((WORD)(lcid))
#define SORTIDFROMLCID(lcid)   ((WORD)((((DWORD)(lcid)) >> 16) & 0xf))

#define LANG_NEUTRAL                     0x00
#define LANG_INVARIANT                   0x7f
#define SUBLANG_NEUTRAL                  0x00    // language neutral
#define SUBLANG_DEFAULT                  0x01    // user default
#define SORT_DEFAULT                     0x0     // sorting default
#define SUBLANG_SYS_DEFAULT              0x02    // system default

#define MAKELANGID(p, s)       ((((WORD  )(s)) << 10) | (WORD  )(p))
#define PRIMARYLANGID(lgid)    ((WORD  )(lgid) & 0x3ff)
#define SUBLANGID(lgid)        ((WORD  )(lgid) >> 10)

#define LANG_SYSTEM_DEFAULT    (MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT))
#define LANG_USER_DEFAULT      (MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT))
#define LOCALE_SYSTEM_DEFAULT  (MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT))
#define LOCALE_USER_DEFAULT    (MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT))
#define LOCALE_NEUTRAL         (MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT))
#define LOCALE_US_ENGLISH      (MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT))
#define LOCALE_INVARIANT       (MAKELCID(MAKELANGID(LANG_INVARIANT, SUBLANG_NEUTRAL), SORT_DEFAULT))

#define SUBLANG_ENGLISH_US               0x01
#define SUBLANG_CHINESE_TRADITIONAL      0x01    /* Chinese (Traditional) */