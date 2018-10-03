// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "WellKnownMethodInfo.h"

namespace Agent
{
    namespace Reflection
    {
        CWellKnownMethodInfo::CWellKnownMethodInfo(
            _In_ std::wstring assemblyName,
            _In_ std::wstring moduleName,
            _In_ std::wstring typeName,
            _In_ std::wstring methodName,
            _In_ PCCOR_SIGNATURE pCorSig,
            _In_ ULONG ulCorSigSize)
            : m_spTypeInfo(new CWellKnownTypeInfo(assemblyName, moduleName, typeName))
            , m_methodName(methodName)
            , m_vecMethodSig(pCorSig, pCorSig + ulCorSigSize)
        {
        }

        const std::wstring& CWellKnownMethodInfo::GetMethodName() const
        {
            return m_methodName;
        }

        const std::shared_ptr<CWellKnownTypeInfo> CWellKnownMethodInfo::GetType() const
        {
            return m_spTypeInfo;
        }

        const CorSigDefinition& CWellKnownMethodInfo::GetMethodSig() const
        {
            return m_vecMethodSig;
        }
    }
}