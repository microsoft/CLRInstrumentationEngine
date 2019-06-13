// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

namespace Agent
{
    namespace Interop
    {
        template<typename TInterfaceType, typename TReturnType, typename... TParameters>
        union UPointerToMethodCast
        {
            inline UPointerToMethodCast(TReturnType(__stdcall TInterfaceType::* pMethod)(TParameters...))
                : m_pMethod(pMethod)
            {
            }

            TReturnType(__stdcall TInterfaceType::*m_pMethod)(TParameters...);
            void const * const	LpVoid;
            const UINT_PTR		UintPtr;

            static_assert(
                sizeof(decltype(LpVoid)) == sizeof(decltype(m_pMethod)),
                "Method pointer does not fit into regular pointer, undefined behavior possible!");
        };

        template<typename TInterfaceType, typename TReturnType, typename... TParameters>
        inline UINT_PTR MethodPtrToUintPtr(
            TReturnType(__stdcall TInterfaceType::*pMethod)(TParameters...))
        {
            UPointerToMethodCast<TInterfaceType, TReturnType, TParameters...> thunk(pMethod);
            return thunk.UintPtr;
        }

    }
}