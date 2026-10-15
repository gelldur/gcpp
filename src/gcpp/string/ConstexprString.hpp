// SPDX-License-Identifier: MIT
// Copyright © 2014, 2017 David Sanders
// Copyright © 2019 Dawid Drozd
// Copyright © 2026 Claude AI spiced up

#pragma once

#include <algorithm>
#include <array>
#include <charconv>
#include <concepts>
#include <cstddef>
#include <format>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <version>

#if defined(__cpp_lib_define_static) && __cpp_lib_define_static >= 202506L
#	include <meta>
/// 1 when C++26 std::define_static_string (P3491) is available, e.g. GCC 16 with -std=c++26.
#	define GCPP_HAS_DEFINE_STATIC 1
#else
#	define GCPP_HAS_DEFINE_STATIC 0
#endif

/**
 * @file
 * Compile-time string building. Every operation is `consteval`, so intermediate results exist only
 * inside the compiler. Only a string you explicitly pin into static storage (@ref STATIC_VIEW,
 * @ref STATIC_C_STR) can reach the binary - and it is shrunk to its exact length first.
 *
 * Inspiration: https://akrzemi1.wordpress.com/2017/06/28/compile-time-string-concatenation/
 * Based on:    https://gist.github.com/dsanders11/8951887 (big thanks)
 *
 * @code
 * using namespace gcpp::string;
 * // Only "Hello, World!" lands in .rodata - "Hello", ", ", "World!" do not.
 * constexpr std::string_view greeting = STATIC_VIEW<ConstexprString{"Hello"} + ", " + "World!">;
 * @endcode
 *
 * Design rule - a ConstexprString *value* is a compile-time builder, not a runtime string. The
 * only intended exits to runtime are @ref STATIC_VIEW, @ref STATIC_C_STR (and the C++26
 * `staticView()`), which emit exactly the text, once per program. Everything that would let a
 * ConstexprString object itself reach runtime is a compile error:
 * - formatting / streaming it (std::format, std::println, operator<<): a runtime call needs a
 *   runtime object, i.e. the whole buffer *including unused capacity* emitted per call site,
 * - `toString()` was removed for the same reason (`std::string{STATIC_VIEW<...>}` instead),
 * - `view()`, `c_str()` and the implicit std::string_view conversion are `consteval` (temporaries
 *   are rejected because the pointer would dangle) and refuse values with unused capacity
 *   (the padding would be emitted). For inspection inside your own consteval helpers use the
 *   pointer-free queries (startsWith, endsWith, contains, find, rfind) which work on any value.
 */

namespace gcpp::string
{

template <std::size_t Capacity>
struct ConstexprString;

namespace detail
{

/**
 * Deliberately not constexpr: calling it inside a consteval function is a compile error whose
 * diagnostic contains this name - the only portable way to get a readable consteval error.
 */
inline void unusedCapacityWouldBeEmbeddedInBinary_pinWith_STATIC_VIEW() { }

/**
 * Raw content of any ConstexprString, capacity ignored. For library internals and consteval
 * helpers whose result never leaves the compiler; prefer the public pointer-free queries.
 */
template <std::size_t N>
consteval std::string_view contentOf(const ConstexprString<N>& text)
{
	return std::string_view{text.chars, text.usedLength};
}

} // namespace detail

/**
 * Null terminated string with compile-time *capacity* and compile-time known *length*, usable in
 * constant expressions and as a non-type template parameter (NTTP).
 *
 * Capacity vs length: built from a literal both are equal (`ConstexprString{"abc"}` is
 * `ConstexprString<3>` of length 3). Operations whose result length depends on content
 * (@ref subString, `erasePath`, a `std::source_location` file name...) keep the capacity and only
 * shrink the length. Thanks to that they are plain `consteval` functions taking normal parameters
 * instead of NTTP gymnastics. Unused capacity never reaches the binary: @ref STATIC_VIEW copies
 * the value into an exactly sized object first.
 *
 * @tparam Capacity max number of characters, without the trailing '\0'.
 *
 * @note Data members are public on purpose: a class type can be an NTTP only when it is
 *       *structural* (all members public, no mutable). Treat them as read only.
 *
 * @note `c_str()`, `view()` and the implicit `std::string_view` conversion return pointers into
 *       this object, so they compile only when nothing bad can reach the binary:
 *       - the object has static storage duration - a `consteval` function may return a pointer
 *         only to such an object, so a temporary or local `constexpr` is rejected (no dangling),
 *       - the object has no unused capacity (`length() == capacity()`), otherwise the padding
 *         would be emitted. Results of subString, fromInteger, `where::*`... must be pinned with
 *         @ref STATIC_VIEW, which copies only the text.
 */
template <std::size_t Capacity>
struct ConstexprString
{
	/// Zero filled, so everything after `usedLength` is '\0' and `c_str()` is always terminated.
	char chars[Capacity + 1]{};
	/// Number of meaningful characters in `chars`.
	std::size_t usedLength = 0;

	consteval ConstexprString() = default;

	/**
	 * Implicit on purpose, allows `ConstexprString{"abc"}` and `template <ConstexprString S>` used
	 * as `foo<"abc">()`.
	 */
	consteval ConstexprString(const char (&text)[Capacity + 1])
		: usedLength{Capacity}
	{
		std::copy_n(text, Capacity, chars);
	}

	/**
	 * Copies text that is not an array literal, e.g. `std::source_location::file_name()`.
	 * Too long text is a compile error (a `throw` is never a constant expression).
	 */
	consteval explicit ConstexprString(const std::string_view text)
		: usedLength{text.size()}
	{
		if(text.size() > Capacity)
		{
			throw std::length_error("ConstexprString: text longer than Capacity");
		}
		std::ranges::copy(text, chars);
	}

	/// Checked access, out of range index is a compile error.
	consteval char operator[](const std::size_t index) const
	{
		if(index >= usedLength)
		{
			throw std::out_of_range("ConstexprString: index out of range");
		}
		return chars[index];
	}

	/**
	 * Must be called on an exactly sized object with static storage duration, see class notes.
	 * @code
	 * static constexpr auto NAME = ConstexprString{"rr"} + "::core";
	 * std::puts(NAME.c_str());                     // OK
	 * std::puts(gcpp::where::fileName().c_str());  // compile error: temporary
	 * @endcode
	 */
	consteval const char* c_str() const
	{
		requireNoUnusedCapacity();
		return chars;
	}

	/// Must be called on an exactly sized object with static storage duration, see class notes.
	consteval std::string_view view() const
	{
		requireNoUnusedCapacity();
		return std::string_view{chars, usedLength};
	}

	/**
	 * Kept implicit because it cannot cause the "whole buffer in binary" mistake: like view() it
	 * compiles only for an exactly sized `static constexpr` object, so it never copies padding nor
	 * dangles. It does *not* make ConstexprString printable - std::format looks up a formatter for
	 * the argument's own type and never converts it.
	 * @code
	 * static constexpr auto NAME = ConstexprString{"rr"} + "::core";
	 * void log(std::string_view text);
	 * log(NAME);                         // OK
	 * log(ConstexprString{"a"} + "b");   // compile error, would dangle - use STATIC_VIEW<...>
	 * @endcode
	 */
	consteval operator std::string_view() const
	{
		requireNoUnusedCapacity();
		return std::string_view{chars, usedLength};
	}

	/// @name Pointer-free queries, usable on any value (temporaries, unused capacity)
	/// @{
	consteval bool startsWith(const std::string_view prefix) const
	{
		return detail::contentOf(*this).starts_with(prefix);
	}

	consteval bool endsWith(const std::string_view suffix) const
	{
		return detail::contentOf(*this).ends_with(suffix);
	}

	consteval bool contains(const std::string_view text) const
	{
		return detail::contentOf(*this).contains(text);
	}

	/// @return position like std::string_view::find, `std::string_view::npos` when not found
	consteval std::size_t find(const std::string_view text, const std::size_t position = 0) const
	{
		return detail::contentOf(*this).find(text, position);
	}

	/// @return position like std::string_view::rfind, `std::string_view::npos` when not found
	consteval std::size_t rfind(const std::string_view text,
								const std::size_t position = std::string_view::npos) const
	{
		return detail::contentOf(*this).rfind(text, position);
	}
	/// @}

	consteval std::size_t length() const
	{
		return usedLength;
	}

	consteval std::size_t size() const
	{
		return usedLength;
	}

	consteval bool empty() const
	{
		return usedLength == 0;
	}

#if GCPP_HAS_DEFINE_STATIC
	/**
	 * Promotes the value to static storage, exactly sized and null terminated. Same guarantees as
	 * @ref STATIC_VIEW (deduplicated, never dangles), but callable on any value - temporaries too:
	 * @code
	 * std::println("{}", where::exceptionTag().staticView());
	 * @endcode
	 */
	consteval std::string_view staticView() const
	{
		return std::string_view{std::define_static_string(detail::contentOf(*this)), usedLength};
	}

	/// Null terminated variant of staticView().
	consteval const char* staticCStr() const
	{
		return std::define_static_string(detail::contentOf(*this));
	}
#else
	/**
	 * Needs C++26 std::define_static_string. In C++23 a member function cannot do this: `*this` is
	 * not a constant expression inside the function, so it cannot become the template argument that
	 * pinning requires. Only the call site can - use `STATIC_VIEW<expression>`.
	 */
	template <bool IsSupported = false>
	consteval std::string_view staticView() const
	{
		static_assert(
			IsSupported,
			"staticView() requires C++26 std::define_static_string (GCC 16+, -std=c++26). "
			"Use gcpp::string::STATIC_VIEW<expression> instead.");
		return {};
	}

	/// See staticView().
	template <bool IsSupported = false>
	consteval const char* staticCStr() const
	{
		static_assert(
			IsSupported,
			"staticCStr() requires C++26 std::define_static_string (GCC 16+, -std=c++26). "
			"Use gcpp::string::STATIC_C_STR<expression> instead.");
		return nullptr;
	}
#endif

	static consteval std::size_t capacity()
	{
		return Capacity;
	}

	/**
	 * Blocked on purpose: streaming needs a runtime object, which would embed the whole buffer
	 * (unused capacity included) at every call site. Stream `STATIC_VIEW<expression>` instead.
	 */
	template <typename Stream>
	friend Stream& operator<<(Stream& stream, const ConstexprString&)
	{
		static_assert(sizeof(Stream) == 0,
					  "Do not stream ConstexprString directly, it would embed the whole buffer in "
					  "the binary. Stream gcpp::string::STATIC_VIEW<expression> instead.");
		return stream;
	}

	/// Compares content only (capacity is irrelevant). `!=` and reversed forms are synthesized.
	template <std::size_t M>
	friend consteval bool operator==(const ConstexprString& lhs, const ConstexprString<M>& rhs)
	{
		return detail::contentOf(lhs) == detail::contentOf(rhs);
	}

	/// Compares with a std::string_view (e.g. STATIC_VIEW). `!=` and reversed forms are
	/// synthesized.
	friend consteval bool operator==(const ConstexprString& lhs, const std::string_view rhs)
	{
		return detail::contentOf(lhs) == rhs;
	}

	/// Compares with a string literal. `!=` and reversed forms are synthesized.
	template <std::size_t M>
	friend consteval bool operator==(const ConstexprString& lhs, const char (&rhs)[M])
	{
		return detail::contentOf(lhs) == std::string_view{rhs, M - 1};
	}

	/// Compile error (with a readable name) when the padding of this object could be emitted.
	consteval void requireNoUnusedCapacity() const
	{
		if(usedLength != Capacity)
		{
			detail::unusedCapacityWouldBeEmbeddedInBinary_pinWith_STATIC_VIEW();
		}
	}
};

/// `ConstexprString{"abc"}` deduces `ConstexprString<3>` (literal size minus '\0').
template <std::size_t M>
ConstexprString(const char (&)[M]) -> ConstexprString<M - 1>;

/// True for any `ConstexprString<N>` (cv-ref qualifiers are stripped).
template <typename T>
concept IsConstexprString = requires(const std::remove_cvref_t<T>& text) {
	[]<std::size_t N>(const ConstexprString<N>&) { }(text);
};

template <std::size_t N, std::size_t M>
consteval ConstexprString<N + M> operator+(const ConstexprString<N>& lhs,
										   const ConstexprString<M>& rhs)
{
	ConstexprString<N + M> result;
	std::copy_n(lhs.chars, lhs.usedLength, result.chars);
	std::copy_n(rhs.chars, rhs.usedLength, result.chars + lhs.usedLength);
	result.usedLength = lhs.usedLength + rhs.usedLength;
	return result;
}

template <std::size_t N, std::size_t M>
consteval ConstexprString<N + M - 1> operator+(const ConstexprString<N>& lhs, const char (&rhs)[M])
{
	return lhs + ConstexprString<M - 1>{rhs};
}

template <std::size_t M, std::size_t N>
consteval ConstexprString<M - 1 + N> operator+(const char (&lhs)[M], const ConstexprString<N>& rhs)
{
	return ConstexprString<M - 1>{lhs} + rhs;
}

/**
 * Factory kept for backward compatibility; equivalent to `ConstexprString{text}`.
 * @code
 * constexpr auto text = make("abc"); // ConstexprString<3>
 * @endcode
 */
template <std::size_t M>
consteval ConstexprString<M - 1> make(const char (&text)[M])
{
	return ConstexprString<M - 1>{text};
}

/**
 * Same semantic as std::string::substr. Capacity is preserved, only the length changes.
 * @code
 * subString(ConstexprString{"abcd"}, 2);    // "cd"
 * subString(ConstexprString{"abcd"}, 1, 2); // "bc"
 * @endcode
 */
template <std::size_t N>
consteval ConstexprString<N> subString(const ConstexprString<N>& text,
									   const std::size_t position,
									   const std::size_t count = std::string_view::npos)
{
	if(position > text.usedLength)
	{
		throw std::out_of_range("subString: position out of range");
	}
	const std::size_t newLength = std::min(count, text.usedLength - position);

	ConstexprString<N> result;
	std::copy_n(text.chars + position, newLength, result.chars);
	result.usedLength = newLength;
	return result;
}

/// Enough for any 64-bit integer: 20 digits of uint64 max, or 19 digits + '-' of int64 min.
inline constexpr std::size_t INTEGER_CAPACITY = 20;

/**
 * Integer to decimal text at compile time, e.g. for `__LINE__` (replaces the `S2(x)` macro trick).
 * Uses `std::to_chars`, which is constexpr for integers since C++23.
 * @code
 * fromInteger(-42); // "-42"
 * @endcode
 */
template <std::integral T>
consteval ConstexprString<INTEGER_CAPACITY> fromInteger(const T value)
{
	static_assert(sizeof(T) <= 8, "fromInteger: up to 64-bit integers supported");

	ConstexprString<INTEGER_CAPACITY> result;
	const auto [end, error] = std::to_chars(result.chars, result.chars + INTEGER_CAPACITY, value);
	result.usedLength = static_cast<std::size_t>(end - result.chars);
	return result;
}

/**
 * Exactly sized, null terminated copy of `S` in static storage - nothing else (no capacity
 * padding, no length field). Backing storage of @ref STATIC_VIEW and @ref STATIC_C_STR.
 */
template <ConstexprString S>
inline constexpr std::array<char, S.length() + 1> STATIC_CHARS = [] {
	std::array<char, S.length() + 1> result{};
	std::ranges::copy(detail::contentOf(S), result.begin());
	return result;
}();

/**
 * Pins a compile-time string into static storage and gives runtime friendly access.
 *
 * Guarantees, by the standard (not by optimizer luck):
 * - The pointee has static storage duration, so the view can never dangle (a `constexpr`
 *   `std::string_view` cannot point anywhere else - it would not compile).
 * - Same value => same object for the whole program: `STATIC_CHARS<S>` is an inline variable
 *   template, the linker keeps one copy across translation units (COMDAT).
 * - Only the final, exactly sized string is emitted; the source literals, intermediate
 *   concatenation results and unused capacity live solely in the compiler.
 *
 * @code
 * constexpr std::string_view text = STATIC_VIEW<ConstexprString{"a"} + "b">; // "ab"
 * @endcode
 */
template <ConstexprString S>
inline constexpr std::string_view STATIC_VIEW{STATIC_CHARS<S>.data(), S.length()};

/// Same as @ref STATIC_VIEW but null terminated `const char*`, handy for C APIs / Qt / printf.
template <ConstexprString S>
inline constexpr const char* STATIC_C_STR = STATIC_CHARS<S>.data();

/**
 * Function spelling of @ref STATIC_VIEW. The string is a *template* argument on purpose: a normal
 * function parameter is never a constant expression (not even in `consteval`), so a
 * `toView(text)` overload could not pin the value - it would need a runtime object and embed the
 * whole buffer, unused capacity included.
 * @code
 * constexpr std::string_view tag = toView<where::exceptionTag()>();
 * @endcode
 */
template <ConstexprString S>
consteval std::string_view toView()
{
	return STATIC_VIEW<S>;
}

/**
 * Runtime std::string initialized from the pinned, exactly sized text - the binary holds only that
 * text (once per program), the std::string itself is an ordinary runtime copy (SSO or heap).
 * See toView() why the string is a template argument.
 * @code
 * throw std::runtime_error(toString<where::exceptionTag()>() + "Order rejected");
 * @endcode
 */
template <ConstexprString S>
std::string toString()
{
	return std::string{STATIC_VIEW<S>};
}

} // namespace gcpp::string

/**
 * Blocked on purpose: formatting runs at runtime, so the argument must be a runtime object - for a
 * temporary like `where::exceptionTag()` that means the whole buffer (1 KB+ of unused capacity)
 * embedded at every call site. Print the pinned text instead, which is exact and shared:
 * @code
 * std::println("{}", gcpp::string::STATIC_VIEW<gcpp::where::exceptionTag()>);
 * @endcode
 */
template <std::size_t Capacity>
struct std::formatter<gcpp::string::ConstexprString<Capacity>, char>
{
	template <typename ParseContext>
	constexpr auto parse(ParseContext& context)
	{
		return context.begin();
	}

	template <typename FormatContext>
	auto format(const gcpp::string::ConstexprString<Capacity>&, FormatContext& context) const
	{
		static_assert(sizeof(FormatContext) == 0,
					  "Do not format ConstexprString directly, it would embed the whole buffer in "
					  "the binary. Format gcpp::string::STATIC_VIEW<expression> instead.");
		return context.out();
	}
};
