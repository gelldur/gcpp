//
// Created by Dawid Drozd aka Gelldur on 10/11/16.
//
#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>

#include "gcpp/string/ConstexprString.hpp"

// clang-format off
/**
 * @file
 * Compile-time type names, replacement of the RTTI + `abi::__cxa_demangle` based `type_name`.
 *
 * How: the compiler already writes the template argument into `__PRETTY_FUNCTION__` (GCC, Clang)
 * or `__FUNCSIG__` (MSVC) of a function template, e.g. GCC:
 *     consteval std::string_view gcpp::detail::rawTypeName() [with T = rr::core::Order]
 * The type is cut out at compile time. Prefix/suffix lengths are not hard coded: they are measured
 * on a probe type (`int`), so the code does not depend on how a compiler decorates the signature.
 *
 * API:
 * - `gcpp::TYPE_NAME<T>`        - ready `std::string_view`, the everyday spelling.
 * - `GCPP_TYPE_NAME_OF(expr)`   - ready `std::string_view` for the (decayed) type of an expression.
 * - `gcpp::typeName<T>()`       - ConstexprString, for compile-time composition:
 *                                 `toString<"Unsupported " + gcpp::typeName<T>()>()`.
 * - `gcpp::typeNameOf(variable)`- ConstexprString of the decayed type of a variable.
 *
 * Why ConstexprString underneath and not a plain `std::string_view` into `__PRETTY_FUNCTION__`:
 * a view into it keeps the *whole signature* alive, so every type name drags its
 * `consteval std::string_view gcpp::detail::rawTypeName() [with T = ...]` text into the binary.
 * Measured for 300 types, `-O2`, stripped binary / .rodata:
 *
 *     | variant                                   | GCC 14             | Clang 18           |
 *     |-------------------------------------------|--------------------|--------------------|
 *     | ConstexprString + STATIC_VIEW (this file) | 22.7 KB /  9.6 KB  | 18.6 KB /  7.0 KB  |
 *     | string_view into __PRETTY_FUNCTION__      | 63.6 KB / 43.1 KB  | 39.1 KB / 22.3 KB  |
 *     | old RTTI + __cxa_demangle                 | 117.0 KB /  9.7 KB | 121.2 KB /  7.0 KB |
 *
 * The cost is compile time only (~2.7 ms per distinct type on GCC 14), nothing at runtime.
 *
 * Compared to the RTTI version: no RTTI (works with `-fno-rtti`), no heap, no `free`, no runtime
 * demangling; cv and reference qualifiers come from the compiler instead of manual appending; the
 * old `type_name<T>()` returning std::string is removed - use `std::string{gcpp::TYPE_NAME<T>}`.
 *
 * Names are fully qualified: every namespace and enclosing class is included, also inside
 * template arguments; aliases resolve to the real type; the global namespace has no prefix.
 *
 * @warning The spelling is compiler specific (each case is pinned by a test, so a compiler
 *          upgrade that changes it fails loudly). Use names for logs and diagnostics, never as a
 *          persistent/serialized key or for comparing types across builds.
 *
 *     | type                                    | GCC 14                                   | Clang 18                                |
 *     |-----------------------------------------|------------------------------------------|-----------------------------------------|
 *     | rr::core::exchange::bybit::Instrument   | rr::core::exchange::bybit::Instrument    | same                                    |
 *     | rr::core::Order::Leg (nested)           | rr::core::Order::Leg                     | same                                    |
 *     | rr::core::OrderAlias (= Order)          | rr::core::Order                          | same                                    |
 *     | std::map<int, rr::core::Order>          | std::map<int, rr::core::Order>           | same                                    |
 *     | struct T in anonymous namespace         | {anonymous}::T                           | (anonymous namespace)::T                |
 *     | rr::core::v2::T (inline namespace v2)   | rr::core::v2::T                          | rr::core::T                             |
 *     | struct T local to main()                | main()::T                                | T              (scope is lost)          |
 *     | lambda in main()                        | main()::<lambda()>                       | (lambda at main.cpp:7:15)               |
 *     | const int&                              | const int&                               | const int &                             |
 *     | const rr::core::Order*                  | const rr::core::Order*                   | const rr::core::Order *                 |
 *     | std::string                             | std::__cxx11::basic_string<char>         | std::basic_string<char>                 |
 *     | std::chrono::system_clock::time_point   | std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long int, std::ratio<1, 1000000000> > > |
 *     |                                         |                                          | std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long, std::ratio<1, 1000000000>>> |
 *
 *     Notes: inline namespaces such as libstdc++'s `std::__cxx11` / `std::chrono::_V2` show up
 *     only on GCC; `long int` vs `long`; `> >` vs `>>`. Default template arguments are omitted by
 *     both (the old demangler printed `std::vector<int, std::allocator<int> >`).
 *     MSVC (untested) additionally prefixes `struct ` / `class ` / `enum ` and spells `__int64`.
 */
// clang-format on

namespace gcpp::detail
{

/// Signature of this function contains the spelled out `T`. Pointer to a static array, so it is
/// fine to return it from a consteval function.
template <typename T>
consteval std::string_view rawTypeName()
{
#if defined(__clang__) || defined(__GNUC__)
	return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
	return __FUNCSIG__;
#else
#	error "gcpp::typeName: unsupported compiler"
#endif
}

struct TypeNameLayout
{
	std::size_t prefix = 0;
	std::size_t suffix = 0;
};

/// Measures the decoration around the type name using a probe type with a known spelling.
consteval TypeNameLayout typeNameLayout()
{
	constexpr std::string_view probe = "int";
	const std::string_view raw = rawTypeName<int>();
	// rfind: the type is spelled at the end, the function name part could contain "int"
	const std::size_t position = raw.rfind(probe);
	if(position == std::string_view::npos)
	{
		throw "gcpp::typeName: unexpected __PRETTY_FUNCTION__ format";
	}
	return TypeNameLayout{position, raw.size() - position - probe.size()};
}

/**
 * Name of `T` as spelled by the compiler, as an exactly sized ConstexprString.
 * Because the capacity equals the length, the result also works with `.view()` on a
 * `static constexpr` copy.
 * @code
 * static_assert(gcpp::typeName<rr::core::Order>() == "rr::core::Order");
 * std::println("{}", gcpp::string::toView<gcpp::typeName<T>()>());
 * auto text = gcpp::string::toString<"Unsupported " + gcpp::typeName<T>()>();
 * @endcode
 */
template <typename T>
consteval auto typeName()
{
	constexpr detail::TypeNameLayout layout = detail::typeNameLayout();
	constexpr std::string_view raw = detail::rawTypeName<T>();
	constexpr std::string_view name =
		raw.substr(layout.prefix, raw.size() - layout.prefix - layout.suffix);

	return gcpp::string::ConstexprString<name.size()>{name};
}

} // namespace gcpp::detail

namespace gcpp
{

/**
 * Name of `T` as a ready `std::string_view`: exactly sized, null terminated, one copy per program,
 * never dangles. Use typeName<T>() only when composing a longer compile-time string.
 * @code
 * std::println("unsupported {}", gcpp::TYPE_NAME<T>);
 * std::string name{gcpp::TYPE_NAME<rr::core::Order>}; // "rr::core::Order"
 * @endcode
 */
template <typename T>
inline constexpr std::string_view TYPE_NAME =
	gcpp::string::STATIC_VIEW<gcpp::detail::typeName<T>()>;

template <typename T>
consteval std::string_view typeName()
{
	return TYPE_NAME<T>;
}

template <typename T>
consteval std::string_view typeNameOf(const T& /*variable*/)
{
	return TYPE_NAME<std::decay_t<T>>;
}

} // namespace gcpp

/**
 * `std::string_view` (exactly sized, static, deduplicated) with the decayed type name of any
 * expression - reference variables and parameters too, because it only uses `decltype`; the
 * expression is never evaluated. A macro because `decltype` of a caller's expression cannot be
 * taken inside a function. For compile-time composition use
 * `gcpp::typeName<std::decay_t<decltype(x)>>()` instead.
 * @code
 * void log(const auto& value) { std::println("{}", GCPP_TYPE_NAME_OF(value)); }
 * @endcode
 */
#define GCPP_TYPE_NAME_OF(...) ::gcpp::TYPE_NAME<::std::decay_t<decltype(__VA_ARGS__)>>
