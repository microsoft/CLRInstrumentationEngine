// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "WellKnownTypeInfo.h"

namespace Agent
{
    namespace Reflection
    {
        CWellKnownTypeInfo::CWellKnownTypeInfo(_In_ const std::wstring& assemblyName, _In_ const std::wstring& moduleName, _In_ const std::wstring& typeName)
            : m_assemblyName(assemblyName)
            , m_moduleName(moduleName)
            , m_typeName(typeName)
        {
        }

        const std::wstring& CWellKnownTypeInfo::GetAssemblyName() const
        {
            return m_assemblyName;
        }

        const std::wstring& CWellKnownTypeInfo::GetModuleName() const
        {
            return m_moduleName;
        }

        const std::wstring& CWellKnownTypeInfo::GetTypeName() const
        {
            return m_typeName;
        }
    }
}
