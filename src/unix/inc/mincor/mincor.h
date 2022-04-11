#pragma once
/*
typedef struct _IMAGE_DATA_DIRECTORY {
    DWORD VirtualAddress;
    DWORD Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;
*/

/*
typedef UINT32 mdToken;

typedef mdToken mdModule;

typedef SIZE_T mdScope;

typedef mdToken mdTypeDef;

typedef mdToken mdSourceFile;

typedef mdToken mdMemberRef;

typedef mdToken mdMethodDef;

typedef mdToken mdFieldDef;

typedef mdToken mdSignature;
*/

typedef /* [public][public][public][public] */ struct __MIDL___MIDL_itf_corprof_0000_0000_0001
    {
    DWORD dwOSPlatformId;
    DWORD dwOSMajorVersion;
    DWORD dwOSMinorVersion;
    }   OSINFO;

typedef /* [public][public][public] */ struct __MIDL___MIDL_itf_corprof_0000_0000_0002
    {
    USHORT usMajorVersion;
    USHORT usMinorVersion;
    USHORT usBuildNumber;
    USHORT usRevisionNumber;
    LPWSTR szLocale;
    ULONG cbLocale;
    DWORD *rProcessor;
    ULONG ulProcessor;
    OSINFO *rOS;
    ULONG ulOS;
    }   ASSEMBLYMETADATA;

typedef struct _IMAGE_DATA_DIRECTORY {
    ULONG   VirtualAddress;
    ULONG   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

typedef WCHAR OLECHAR;
typedef OLECHAR* LPOLESTR;
typedef const OLECHAR* LPCOLESTR;

typedef WCHAR *BSTR;