// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

// C++ 0x11 specification has thread_local as part of it
// but it's not fully supported by the Microsoft C++ compiler
// there's no such keyword yet.
#define  thread_local __declspec(thread)

namespace Agent
{
namespace Threading
{
namespace Tls
{

	template<typename T> class CDeclspecVariablePointerContainerImplT final
	{
		static thread_local T* m_pVariable;
	public:
		_Ret_notnull_ const T* Assign(const T& var)
		{
			_ASSERT_EXPR(
				nullptr == m_pVariable,
				L"Varible already initialized, this call may cause a memory leak");

			return Assign(&var);
		}

		_Ret_notnull_ const T* Assign(_In_ T* const var)
		{
			_ASSERT_EXPR(
				nullptr == m_pVariable,
				L"Varible already initialized, this call may cause a memory leak");

			m_pVariable = var;

			return m_pVariable;
		}

		_Ret_notnull_ T* const Get() const
		{
			return m_pVariable;
		}

		_Ret_notnull_ T* Release()
		{
			_ASSERT_EXPR(
				nullptr != m_pVariable,
				L"Varible already detached, this call may be the result of wrong call sequence");

			auto pVar = m_pVariable;
			m_pVariable = nullptr;

			return pVar;
		}
	}; //CDeclspecVariableContainerImplT

	template<typename T> thread_local T* CDeclspecVariablePointerContainerImplT<T>::m_pVariable =
		decltype(CDeclspecVariablePointerContainerImplT<T>::m_pVariable)();

	template <typename T> using CVariablePointerContainer = CDeclspecVariablePointerContainerImplT < typename T >;

	template<typename T, const T DefaultValue> class CDeclspecVariableContainerImplT final
	{
		static thread_local T m_var;
	public:
		static const T DefaultValue = DefaultValue;

		const T& Assign(const T& var)
		{
			m_var = var;

			return m_var;
		}

		const T& Get() const
		{
			return m_var;
		}

		T Release()
		{
			auto var = m_var;
			m_var = DefaultValue;

			return var;
		}
	}; //CDeclspecVariableContainerImplT

	template<
		typename T,
		const T DefaultValue
	> thread_local T CDeclspecVariableContainerImplT<T, DefaultValue>::m_var = DefaultValue;

	template <
		typename T,
		const T DefaultValue
	> using CVariableContainer = CDeclspecVariableContainerImplT < typename T, DefaultValue >;

} // Tls
} // Threading
} // Agent