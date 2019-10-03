// SPDX-License-Identifier: MIT
// Copyright © 2014, 2017 David Sanders
// Copyright © 2019 Dawid Drozd aka Gelldur
#pragma once

#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

/**
 * Inspiration: https://akrzemi1.wordpress.com/2017/06/28/compile-time-string-concatenation/
 * Based on:    https://gist.github.com/dsanders11/8951887 (big thanks)
 *
 *
 * Motivation: it would be nice to have such define: LOCATION3
 *
 * #define S1(x) #x
 * #define S2(x) S1(x)
 * #define LOCATION __FILE__ ":" S2(__LINE__)
 * #define LOCATION2 __PRETTY_FUNCTION__ ":" S2(__LINE__) <-- not working!!!
 * #define LOCATION3 string::make(__PRETTY_FUNCTION__) + ":" + string::make(S2(__LINE__))
 */

namespace string
{

// Recursive comparison of each individual character in a string
// The last bit with std::enable_if uses SFINAE (Substitution Failure Is Not An Error)
// to rule this function out and switch to the base case for the recursion when the Index == Length
template <std::size_t Length, std::size_t Index, typename Left, typename Right>
constexpr auto compareCharacters(const Left& lhs, const Right& rhs) ->
	typename std::enable_if<Index != Length, bool>::type
{
	return lhs[Index] == rhs[Index] ? compareCharacters<Length, Index + 1>(lhs, rhs) : false;
}

// Recursion base case. If you run past the last index of
template <std::size_t Length,
		  std::size_t Index,
		  typename Left,
		  typename Right,
		  typename std::enable_if<Index == Length, bool>::type = 0>
constexpr bool compareCharacters(const Left& lhs, const Right& rhs)
{
	return true;
}

// Helper type traits to determine the length of either
// a string literal or a StringConstant (specialized below)
template <typename T>
struct length_of
{
	static_assert(std::is_void<T>::value, "Unsupported type for length_of");

	static constexpr std::size_t value = 1;
};

template <std::size_t N>
struct length_of<const char (&)[N]>
{
	static constexpr std::size_t value = N - 1;
};

template <std::size_t N>
struct length_of<char[N]>
{
	static constexpr std::size_t value = N - 1;
};

template <std::size_t N>
struct length_of<const char[N]>
{
	static constexpr std::size_t value = N - 1;
};

// This small class is the heart of the constant string implementation.
// It has constructors for string literals and individual chars, as well
// as operators to interact with string literals or other instances. This
// allows for it to have a very natural interface, and it's all constexpr
// Inspired heavily by a class described in a presentation by Scott Schurr
// at Boostcon:
// https://github.com/boostcon/cppnow_presentations_2012/blob/master/wed/schurr_cpp11_tools_for_class_authors.pdf
template <std::size_t N>
class ConstexprString
{
public:
	// Constructor which takes individual chars. Allows for unpacking
	// parameter packs directly into the constructor
	template <typename... Characters>
	constexpr explicit ConstexprString(Characters... characters)
		: _value{characters..., '\0'}
	{}

	// Copy constructor
	template <std::size_t... Indexes>
	constexpr ConstexprString(const ConstexprString<N>& rhs,
							  std::index_sequence<Indexes...> dummy = ConstexprString::g_indexes)
		: _value{rhs[Indexes]..., '\0'}
	{}

	template <std::size_t X, std::size_t... Indexes>
	constexpr ConstexprString(const ConstexprString<X>& rhs,
							  std::index_sequence<Indexes...> /*dummy*/)
		: _value{rhs[Indexes]..., '\0'}
	{}

	template <std::size_t... Indexes>
	constexpr ConstexprString(const char (&value)[N + 1], std::index_sequence<Indexes...> /*dummy*/)
		: ConstexprString(value[Indexes]...)
	{}

	constexpr explicit ConstexprString(const char (&value)[N + 1])
		: ConstexprString(value, std::make_index_sequence<N>{})
	{}

	// Array subscript operator, with some basic range checking
	constexpr char operator[](const std::size_t index) const
	{
		return index < N ? _value[index] : throw std::out_of_range("Index out of range");
	}

	constexpr const char* c_str() const
	{
		return _value;
	}

	constexpr std::size_t length() const
	{
		return N;
	}

	std::string toString() const
	{
		return std::string(_value);
	}

	friend std::ostream& operator<<(std::ostream& os, const ConstexprString& constant)
	{
		os << constant._value;
		return os;
	}

protected:
	const char _value[N + 1];

	static constexpr auto g_indexes = typename std::make_index_sequence<N>{};
};

// Specialize the length_of trait for the ConstexprString class
template <std::size_t N>
struct length_of<ConstexprString<N>>
{
	static constexpr std::size_t value = N;
};

template <std::size_t N>
struct length_of<const ConstexprString<N>>
{
	static constexpr std::size_t value = N;
};

template <std::size_t N>
struct length_of<const ConstexprString<N>&>
{
	static constexpr std::size_t value = N;
};

// A helper trait for checking if something is a ConstexprString
// without having to know the length of the string it contains
template <typename T>
struct is_string_constant
{
	static constexpr bool value = false;
};

template <std::size_t N>
struct is_string_constant<ConstexprString<N>>
{
	static constexpr bool value = true;
};

template <std::size_t N>
struct is_string_constant<ConstexprString<N>&>
{
	static constexpr bool value = true;
};

template <std::size_t N>
struct is_string_constant<const ConstexprString<N>>
{
	static constexpr bool value = true;
};

template <std::size_t N>
struct is_string_constant<const ConstexprString<N>&>
{
	static constexpr bool value = true;
};

// A helper function for concatenating ConstexprStrings

// Less than human friendly concat function, wrapped by a human friendly one below
template <typename Left, typename Right, std::size_t... IndexesLeft, std::size_t... IndexesRight>
constexpr ConstexprString<sizeof...(IndexesLeft) + sizeof...(IndexesRight)>
ConcatStrings(const Left& lhs,
			  const Right& rhs,
			  std::index_sequence<IndexesLeft...> /*dummy1*/,
			  std::index_sequence<IndexesRight...> /*dummy2*/)
{
	return ConstexprString<sizeof...(IndexesLeft) + sizeof...(IndexesRight)>(lhs[IndexesLeft]...,
																			 rhs[IndexesRight]...);
}

// Human friendly concat function for string literals
template <typename Left, typename Right>
constexpr ConstexprString<length_of<Left>::value + length_of<Right>::value>
concatStrings(const Left& lhs, const Right& rhs)
{
	return ConcatStrings(lhs,
						 rhs,
						 typename std::make_index_sequence<length_of<decltype(lhs)>::value>{},
						 typename std::make_index_sequence<length_of<decltype(rhs)>::value>{});
}

// Finally, operators for dealing with a string literal LHS and ConstexprString RHS

// Addition operator
template <std::size_t N, typename Right>
constexpr ConstexprString<N + length_of<Right>::value> operator+(const ConstexprString<N>& lhs,
																 const Right& rhs)
{
	return concatStrings(lhs, rhs);
}

template <typename Left, std::size_t N>
constexpr ConstexprString<length_of<Left>::value + N> operator+(const Left& lhs,
																const ConstexprString<N>& rhs)
{
	return concatStrings(lhs, rhs);
}

template <std::size_t X, std::size_t Y>
constexpr ConstexprString<X + Y> operator+(const ConstexprString<X>& lhs,
										   const ConstexprString<Y>& rhs)
{
	return concatStrings(lhs, rhs);
}

// Equality operator
template <std::size_t N, typename Right>
constexpr auto operator==(const ConstexprString<N>& lhs, const Right& rhs) ->
	typename std::enable_if<N == length_of<Right>::value, bool>::type
{
	return compareCharacters<N, 0>(lhs, rhs);
}

template <std::size_t N, typename Right>
constexpr auto operator!=(const ConstexprString<N>& lhs, const Right& rhs) ->
	typename std::enable_if<N == length_of<Right>::value, bool>::type
{
	return !(lhs == rhs);
}

template <typename Left, std::size_t N>
constexpr auto operator==(const Left& lhs, const ConstexprString<N>& rhs) ->
	typename std::enable_if<length_of<Left>::value == N, bool>::type
{
	return compareCharacters<N, 0>(lhs, rhs);
}

template <typename Left, std::size_t N>
constexpr auto operator!=(const Left& lhs, const ConstexprString<N>& rhs) ->
	typename std::enable_if<length_of<Left>::value == N, bool>::type
{
	return !(lhs == rhs);
}

template <std::size_t X, std::size_t Y>
constexpr auto operator==(const ConstexprString<X>& lhs, const ConstexprString<Y>& rhs) ->
	typename std::enable_if<X == Y, bool>::type
{
	return compareCharacters<X, 0>(lhs, rhs);
}

template <std::size_t X, std::size_t Y>
constexpr auto operator!=(const ConstexprString<X>& lhs, const ConstexprString<Y>& rhs) ->
	typename std::enable_if<X == Y, bool>::type
{
	return !(lhs == rhs);
}

// Different length strings can never be equal
template <std::size_t N,
		  typename Right,
		  typename std::enable_if<N != length_of<Right>::value, bool>::type = 0>
constexpr bool operator==(const ConstexprString<N>& lhs, const Right& rhs)
{
	return false;
}

template <std::size_t N,
		  typename Right,
		  typename std::enable_if<N != length_of<Right>::value, bool>::type = 0>
constexpr bool operator!=(const ConstexprString<N>& lhs, const Right& rhs)
{
	return !(lhs == rhs);
}

// Different length strings can never be equal
template <typename Left,
		  std::size_t N,
		  typename std::enable_if<length_of<Left>::value != N, bool>::type = 0>
constexpr bool operator==(const Left& lhs, const ConstexprString<N>& rhs)
{
	return false;
}

template <typename Left,
		  std::size_t N,
		  typename std::enable_if<length_of<Left>::value != N, bool>::type = 0>
constexpr bool operator!=(const Left& lhs, const ConstexprString<N>& rhs)
{
	return !(lhs == rhs);
}

// Different length strings can never be equal
template <std::size_t X, std::size_t Y, typename std::enable_if<X != Y, bool>::type = 0>
constexpr bool operator==(const ConstexprString<X>& lhs, const ConstexprString<Y>& rhs)
{
	return false;
}

template <std::size_t X, std::size_t Y, typename std::enable_if<X != Y, bool>::type = 0>
constexpr bool operator!=(const ConstexprString<X>& lhs, const ConstexprString<Y>& rhs)
{
	return !(lhs == rhs);
}

template <std::size_t N, std::size_t... Indexes>
constexpr auto make(const char (&value)[N], std::index_sequence<Indexes...> /*dummy*/)
{
	return ConstexprString<N - 1>(value[Indexes]...);
}

template <std::size_t N, std::size_t... Indexes>
constexpr auto make(const char* value, std::index_sequence<Indexes...> /*dummy*/)
{
	return ConstexprString<N - 1>(value[Indexes]...);
}

// A helper factory function for creating FixedConstexprString objects
// which handles figuring out the length of the string for you
template <std::size_t N>
constexpr auto make(const char (&value)[N])
{
	return make(value, typename std::make_index_sequence<N - 1>{});
}

template <std::size_t N>
constexpr auto make(const char* value)
{
	return make<N>(value, typename std::make_index_sequence<N - 1>{});
}

template <const std::size_t position, std::size_t N>
constexpr auto subString(ConstexprString<N> text)
{
	static_assert(position < text.length(), "Out of range");
	constexpr auto newSize = text.length() - position + 1;

	return make<newSize>(text.get() + position);
}

} // namespace string
