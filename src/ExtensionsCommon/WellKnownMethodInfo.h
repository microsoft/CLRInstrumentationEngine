// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "WellKnownTypeInfo.h"
typedef std::vector<COR_SIGNATURE> CorSigDefinition;


namespace Agent
{
    namespace Reflection
    {
        class CWellKnownMethodInfo final
        {
        private:
            const std::wstring m_methodName;
            const std::shared_ptr<CWellKnownTypeInfo> m_spTypeInfo;
            const CorSigDefinition	m_vecMethodSig;

        public:
            CWellKnownMethodInfo(
                _In_ const std::wstring& assemblyName,
                _In_ const std::wstring& moduleName,
                _In_ const std::wstring& typeName,
                _In_ const std::wstring& methodName,
                _In_reads_(ulCorSigSize) PCCOR_SIGNATURE pCorSig,
                _In_ ULONG ulCorSigSize);

            const std::wstring& GetMethodName() const;

            const std::shared_ptr<CWellKnownTypeInfo> GetType() const;

            const CorSigDefinition& GetMethodSig() const;
        };
    }
}
