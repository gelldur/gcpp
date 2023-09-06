//
// Created by dddd on 06/09/23.
//
#pragma once

#include <tuple>

namespace gcpp::meta
{
//Checkout: https://stackoverflow.com/questions/22863090/how-to-correctly-find-out-the-return-type-of-a-lambda
template <typename T>
struct return_type : return_type<decltype(&T::operator())>{};
// For generic types, directly use the result of the signature of its 'operator()'

template <typename ClassType, typename ReturnType, typename... Args>
struct return_type<ReturnType (ClassType::*)(Args...) const>
{
	using type = ReturnType;
};

// Example use:
// using Return_t = return_type<decltype(callback)>;

////////////////////////////////////////////////////////////////////////////////////////////////////

//Checkout: https://stackoverflow.com/questions/55649942/get-argument-type-of-lambda-with-auto-specifier/77046644#77046644
namespace details
{
//! Specialization for Function-Pointers
template <typename Ret, typename... Args>
std::tuple<Args...> get_args(Ret (*)(Args...));

//! Specialization for Functor/Lambdas
template <typename F, typename Ret, typename... Args>
std::tuple<Args...> get_args(Ret (F::*)(Args...));

//! Specialization for Functor/Lambdas
template <typename F, typename Ret, typename... Args>
std::tuple<Args...> get_args(Ret (F::*)(Args...) const);

template <typename F>
decltype(get_args(&F::operator())) get_args(F);

}; // namespace details

template <typename F, std::size_t I>
using get_arg = std::tuple_element_t<I, decltype(details::get_args(std::declval<F>()))>;

/*
    Example use:

	template <class EventCallback,
			  typename FromState_t = get_arg<EventCallback, 0>,
			  typename Event_t = get_arg<EventCallback, 1>>
	constexpr void deduce_types_from_callback(EventCallback&& callback)
	{}
 */

////////////////////////////////////////////////////////////////////////////////////////////////////

}
