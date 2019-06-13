// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

namespace Agent
{
namespace Diagnostics
{
    template<typename TStream>
    inline TStream& Format(TStream& ssFmt)
    {
        return ssFmt;
    }

    template<typename TStream, typename TArg, typename... Args>
    inline TStream& Format(
        TStream& ssFmt,
        TArg&& arg,
        Args&&... args)
    {
        ssFmt << arg;
        return Format(ssFmt, std::forward<Args>(args)...);
    }

    inline std::tuple<wchar_t const * const, const std::wstring> Param(
        wchar_t const * const name,
        const std::wstring& value)
    {
        return std::make_tuple(name, value);
    }

    inline std::tuple<wchar_t const * const, wchar_t const * const> Param(
        wchar_t const * const name,
        wchar_t const * const value)
    {
        return std::make_tuple(name, value);
    }

    inline std::tuple<wchar_t const * const, const std::size_t> Param(
        wchar_t const * const name,
        std::size_t value)
    {
        return std::make_tuple(name, value);
    }


    template<typename TStream, typename TParamName, typename TParamValue>
    inline TStream& operator << (
        TStream& ostr,
        const std::tuple<const TParamName, const TParamValue>& tpl)
    {
        return static_cast<TStream&>(ostr << L'[' << std::get<0>(tpl) << L'=' << std::get<1>(tpl) << L"];");
    }

    template<typename... Args>
    inline std::wstring FormatToString(Args&&... args)
    {
        std::wostringstream ssFmt;
        return Format(ssFmt, std::forward<Args>(args)...).str();
    }
}
}