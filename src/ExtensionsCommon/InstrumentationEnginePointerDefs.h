// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

typedef ATL::CComPtr<IUnknown> IUnknownSptr;

typedef ATL::CComPtr<IAppDomainCollection> IAppDomainCollectionSptr;
typedef ATL::CComPtr<IEnumAppDomainInfo> IEnumAppDomainInfoSptr;
typedef ATL::CComPtr<IAppDomainInfo> IAppDomainInfoSptr;

typedef ATL::CComPtr<IAssemblyInfo> IAssemblyInfoSptr;

typedef ATL::CComPtr<IEnumModuleInfo> IEnumModuleInfoSptr;
typedef ATL::CComPtr<IModuleInfo> IModuleInfoSptr;

typedef ATL::CComPtr<IMethodInfo> IMethodInfoSptr;
typedef ATL::CComPtr<IEnumMethodParameters> IEnumMethodParametersSptr;
typedef ATL::CComPtr<IMethodParameter> IMethodParameterSptr;
typedef ATL::CComPtr<ILocalVariableCollection> ILocalVariableCollectionSptr;

typedef ATL::CComPtr<IType> ITypeSptr;
typedef ATL::CComPtr<ITokenType> ITokenTypeSptr;
typedef ATL::CComPtr<ITypeCreator> ITypeCreatorSptr;

typedef ATL::CComPtr<IInstructionFactory> IInstructionFactorySptr;
typedef ATL::CComPtr<IInstructionGraph> IInstructionGraphSptr;
typedef ATL::CComPtr<IInstruction>		IInstructionSptr;

typedef ATL::CComPtr<IExceptionSection> IExceptionSectionSptr;
typedef ATL::CComPtr<IExceptionClause> IExceptionClauseSptr;

typedef ATL::CComPtr<IMetaDataImport2> IMetaDataImportSptr;
typedef ATL::CComPtr<IMetaDataAssemblyImport> IMetaDataAssemblyImportSptr;

typedef ATL::CComPtr<IMetaDataEmit> IMetaDataEmitSptr;
typedef ATL::CComPtr<IMetaDataAssemblyEmit> IMetaDataAssemblyEmitSptr;

typedef ATL::CComPtr<IDataContainer> IDataContainerSptr;

typedef ATL::CComPtr<ICorProfilerInfo3> ICorProfilerInfoSptr;
typedef ATL::CComQIPtr<ICorProfilerInfo3> ICorProfilerInfoQiSptr;

typedef ATL::CComPtr<IInstrumentationMethod> IInstrumentationMethodSptr;