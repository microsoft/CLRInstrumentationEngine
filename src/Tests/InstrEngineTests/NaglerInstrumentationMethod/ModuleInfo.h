//
// (c) Copyright IBM Corp. 2021
// (c) Copyright Instana Inc. 2021
//
#pragma once
#include <corprof.h>
#include <map>
#include <vector>
#include "stdafx.h"

struct ModuleInfo
{
    WCHAR                               m_wszModulePath[512];
    WCHAR								m_assemblyName[255];
    IMetaDataImport* m_pImport;
    IMetaDataAssemblyImport* m_pAssemblyImport;
    mdToken                             m_mdEnterProbeRef;
    mdToken                             m_mdExitProbeRef;
    mdToken                             m_mdHttpEventProbeRef;
    mdTypeRef    						m_objectTypeRef;
    mdTypeRef							m_exceptionTypeRef;
    mdTypeRef							m_int16TypeRef;
    mdTypeRef							m_int32TypeRef;
    mdTypeRef							m_int64TypeRef;
    mdTypeRef							m_float32TypeRef;
    mdTypeRef							m_float64TypeRef;
    mdTypeRef							m_uint16TypeRef;
    mdTypeRef							m_uint32TypeRef;
    mdTypeRef							m_uint64TypeRef;
    mdTypeRef							m_byteTypeRef;
    mdTypeRef							m_signedByteTypeRef;
    mdTypeRef							m_boolTypeRef;
    mdTypeRef							m_apiControllerRef;
    mdTypeRef							m_wcfServiceContractAttributeRef;
    mdTypeRef							m_wcfOperationContractAttributeRef;
    mdTypeRef							m_intPtrTypeRef;
    BOOL								m_resolvedSuccessfully;
    LPCWSTR								m_inheritanceClassName;
};