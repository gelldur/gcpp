//
// Created by Dawid Drozd aka Gelldur on 03.12.17.
//
#pragma once

#include <type_traits>

namespace gcpp::math
{

template <class T, class = void>
struct supports_arithmetic_operations : std::false_type
{};

template <class T>
struct supports_arithmetic_operations<T,
									  std::void_t<decltype(std::declval<T>() + std::declval<T>()),
												  decltype(std::declval<T>() - std::declval<T>()),
												  decltype(std::declval<T>() * std::declval<T>()),
												  decltype(std::declval<T>() / std::declval<T>())>>
	: std::true_type
{};

} // namespace gcpp::math
