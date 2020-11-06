// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Agent
{
    namespace Reflection
    {
        class CWellKnownTypeInfo final
        {
        private:
            const std::wstring m_assemblyName;
            const std::wstring m_moduleName;
            const std::wstring m_typeName;

        public:
            CWellKnownTypeInfo(_In_ const std::wstring& assemblyName, _In_ const std::wstring& moduleName, _In_ const std::wstring& typeName);

            const std::wstring& GetAssemblyName() const;
            const std::wstring& GetModuleName() const;
            const std::wstring& GetTypeName() const;
        };
    }
}
