//
// Created by dddd on 10/15/26.
//
#include <chrono>
#include <format>
#include <map>
#include <memory>
#include <print>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "gcpp/type_name.h"

using gcpp::typeName;
using gcpp::typeNameOf;
using gcpp::string::toString;
using gcpp::string::toView;

struct GlobalType
{ };

namespace
{
struct AnonymousType
{ };
} // namespace

namespace rr::core
{
struct Order
{
	struct Leg
	{ };
	enum class Status
	{
		Open
	};
};
using OrderAlias = Order;
inline namespace v2
{
struct Versioned
{ };
} // namespace v2
namespace
{
struct InnerAnonymous
{ };
} // namespace
enum class SideOption
{
	Call,
	Put
};
template <typename T>
struct Wrapper
{ };
} // namespace rr::core

namespace rr::core::exchange::bybit
{
struct Instrument
{ };
} // namespace rr::core::exchange::bybit

namespace
{

/// Old usage pattern: name of a template parameter inside generic code
template <typename T>
std::string describe(const T&)
{
	return toString<"value of type " + gcpp::detail::typeName<T>()>();
}

/// Reference parameter - the GCPP_TYPE_NAME_OF case
std::string describeParameter(const auto& value)
{
	return "param " + std::string{GCPP_TYPE_NAME_OF(value)};
}

} // namespace

TEST_CASE("Usage example", "[TypeName]")
{
	using T = std::string;
	T value;

	std::println("unsupported {}", gcpp::TYPE_NAME<T>);
	std::string name{gcpp::TYPE_NAME<rr::core::Order>}; // "rr::core::Order"
	std::println("{}", GCPP_TYPE_NAME_OF(value)); // now built on TYPE_NAME
	auto text = toString<"Unsupported " + gcpp::detail::typeName<T>()>(); // only for composing
}

TEST_CASE("typeName of plain and user types", "[TypeName]")
{
	STATIC_REQUIRE(typeName<int>() == "int");
	STATIC_REQUIRE(typeName<double>() == "double");
	STATIC_REQUIRE(typeName<rr::core::Order>() == "rr::core::Order");
	STATIC_REQUIRE(typeName<rr::core::SideOption>() == "rr::core::SideOption");
	STATIC_REQUIRE(typeName<rr::core::Wrapper<int>>() == "rr::core::Wrapper<int>");
	STATIC_REQUIRE(typeName<std::vector<int>>() == "std::vector<int>");
}

TEST_CASE("typeName keeps cv and reference qualifiers", "[TypeName]")
{
	STATIC_REQUIRE(typeName<const int>() == "const int");
	STATIC_REQUIRE(typeName<volatile int>() == "volatile int");
	// spacing before & differs between compilers
	STATIC_REQUIRE(gcpp::detail::typeName<const int&>().startsWith("const int"));
	STATIC_REQUIRE(gcpp::detail::typeName<const int&>().endsWith("&"));
	STATIC_REQUIRE(gcpp::detail::typeName<int&&>().endsWith("&&"));
	STATIC_REQUIRE_FALSE(gcpp::detail::typeName<int&>().endsWith("&&"));
	STATIC_REQUIRE(
		gcpp::detail::typeName<const rr::core::Order*>().startsWith("const rr::core::Order"));
}

TEST_CASE("typeNameOf decays like the old type_name(var)", "[TypeName]")
{
	const int constant = 5;
	std::vector<int> runtimeVector{1, 2, 3}; // runtime value, only the type is used
	int array[3]{};
	const rr::core::Order& reference = rr::core::Order{};

	STATIC_REQUIRE(typeNameOf(constant) == "int");
	STATIC_REQUIRE(typeNameOf(runtimeVector) == "std::vector<int>");
	// reference variable: typeNameOf(reference) needs P2280 (compiler dependent), use the macro
	REQUIRE(GCPP_TYPE_NAME_OF(reference) == "rr::core::Order");
	REQUIRE(GCPP_TYPE_NAME_OF(constant) == "int");
	REQUIRE(GCPP_TYPE_NAME_OF(runtimeVector.size()) == typeName<std::size_t>());
	STATIC_REQUIRE(std::is_same_v<decltype(GCPP_TYPE_NAME_OF(constant)), const std::string_view>);
	// same pinned object as toView
	STATIC_REQUIRE(GCPP_TYPE_NAME_OF(constant).data() ==
				   toView<gcpp::detail::typeName<int>()>().data());
	CHECK(runtimeVector.size() == 3);
}

TEST_CASE("typeName runtime use pins only the exact text", "[TypeName]")
{
	constexpr std::string_view view = toView<gcpp::detail::typeName<rr::core::Order>()>();
	STATIC_REQUIRE(view == "rr::core::Order");
	CHECK(std::format("[{}]", toView<gcpp::detail::typeName<int>()>()) == "[int]");
	CHECK(describe(rr::core::SideOption::Call) == "value of type rr::core::SideOption");
	CHECK(describe(1.5) == "value of type double");
	CHECK(describeParameter(rr::core::Order{}) == "param rr::core::Order");

	static constexpr auto NAME = gcpp::detail::typeName<rr::core::Order>();
	CHECK(NAME.view() == "rr::core::Order"); // allowed: exact size + static storage
}

TEST_CASE("typeName is fully qualified with namespaces", "[TypeName]")
{
	STATIC_REQUIRE(typeName<GlobalType>() == "GlobalType"); // global namespace: no prefix
	STATIC_REQUIRE(typeName<rr::core::Order>() == "rr::core::Order");
	STATIC_REQUIRE(typeName<rr::core::exchange::bybit::Instrument>() ==
				   "rr::core::exchange::bybit::Instrument");
	// nested types keep the enclosing class
	STATIC_REQUIRE(typeName<rr::core::Order::Leg>() == "rr::core::Order::Leg");
	STATIC_REQUIRE(typeName<rr::core::Order::Status>() == "rr::core::Order::Status");
	// alias is resolved to the real, qualified type
	STATIC_REQUIRE(typeName<rr::core::OrderAlias>() == "rr::core::Order");
}

TEST_CASE("typeName qualifies template arguments and compound types", "[TypeName]")
{
	STATIC_REQUIRE(typeName<rr::core::Wrapper<rr::core::Order>>() ==
				   "rr::core::Wrapper<rr::core::Order>");
	STATIC_REQUIRE(typeName<std::unique_ptr<rr::core::Order>>() ==
				   "std::unique_ptr<rr::core::Order>");
	STATIC_REQUIRE(typeName<std::map<int, rr::core::Order::Leg>>() ==
				   "std::map<int, rr::core::Order::Leg>");
	// pointer / reference spacing differs between compilers, the qualified name does not
	STATIC_REQUIRE(
		gcpp::detail::typeName<const rr::core::Order*>().startsWith("const rr::core::Order"));
	STATIC_REQUIRE(gcpp::detail::typeName<rr::core::Order&&>().startsWith("rr::core::Order"));
	STATIC_REQUIRE(gcpp::detail::typeName<std::chrono::system_clock::time_point>().startsWith(
		"std::chrono::time_point<std::chrono::"));
}

TEST_CASE("typeName of anonymous namespace, inline namespace and local types", "[TypeName]")
{
	struct LocalType
	{ };

	// Compiler specific spelling - documented in TypeName.hpp
#if defined(__clang__)
	STATIC_REQUIRE(typeName<AnonymousType>() == "(anonymous namespace)::AnonymousType");
	STATIC_REQUIRE(typeName<rr::core::InnerAnonymous>() ==
				   "rr::core::(anonymous namespace)::InnerAnonymous");
	STATIC_REQUIRE(typeName<rr::core::Versioned>() == "rr::core::Versioned"); // inline ns hidden
	STATIC_REQUIRE(typeName<LocalType>() == "LocalType"); // enclosing function not printed
#elif defined(__GNUC__)
	STATIC_REQUIRE(gcpp::detail::typeName<AnonymousType>() == "{anonymous}::AnonymousType");
	STATIC_REQUIRE(gcpp::detail::typeName<rr::core::InnerAnonymous>() ==
				   "rr::core::{anonymous}::InnerAnonymous");
	STATIC_REQUIRE(gcpp::detail::typeName<rr::core::Versioned>() == "rr::core::v2::Versioned");
	STATIC_REQUIRE(
		gcpp::detail::typeName<LocalType>().endsWith("::LocalType")); // "<function>()::LocalType"
#endif
	STATIC_REQUIRE(gcpp::detail::typeName<rr::core::Versioned>().startsWith("rr::core::"));
	STATIC_REQUIRE(gcpp::detail::typeName<rr::core::InnerAnonymous>().startsWith("rr::core::"));
	STATIC_REQUIRE(gcpp::detail::typeName<rr::core::InnerAnonymous>().endsWith("::InnerAnonymous"));
}

TEST_CASE("typeNameOf and GCPP_TYPE_NAME_OF are qualified too", "[TypeName]")
{
	const rr::core::exchange::bybit::Instrument instrument{};
	const rr::core::Order::Leg& leg = rr::core::Order::Leg{};
	STATIC_REQUIRE(typeNameOf(instrument) == "rr::core::exchange::bybit::Instrument");
	STATIC_REQUIRE(GCPP_TYPE_NAME_OF(leg) == "rr::core::Order::Leg");
	CHECK(std::string{GCPP_TYPE_NAME_OF(instrument)} == "rr::core::exchange::bybit::Instrument");
}

TEST_CASE("TYPE_NAME is a ready std::string_view", "[TypeName]")
{
	STATIC_REQUIRE(std::is_same_v<decltype(gcpp::TYPE_NAME<int>), const std::string_view>);
	STATIC_REQUIRE(gcpp::TYPE_NAME<rr::core::Order::Leg> == "rr::core::Order::Leg");
	// one object per type, shared with every other spelling
	STATIC_REQUIRE(gcpp::TYPE_NAME<rr::core::Order>.data() ==
				   toView<gcpp::detail::typeName<rr::core::Order>()>().data());
	const rr::core::Order order{};
	STATIC_REQUIRE(GCPP_TYPE_NAME_OF(order).data() == gcpp::TYPE_NAME<rr::core::Order>.data());

	CHECK(std::format("unsupported {}", gcpp::TYPE_NAME<rr::core::SideOption>) ==
		  "unsupported rr::core::SideOption");
	CHECK(std::string{gcpp::TYPE_NAME<rr::core::Order>} == "rr::core::Order");
	const auto name = gcpp::TYPE_NAME<rr::core::Order>;
	CHECK(name.data()[name.size()] == '\0'); // valid C string
}

TEST_CASE("typeName compiler specific spellings", "[TypeName]")
{
	auto lambda = [] { };
	using Lambda = decltype(lambda);
#if defined(__clang__)
	STATIC_REQUIRE(typeName<const int&>() == "const int &");
	STATIC_REQUIRE(typeName<const rr::core::Order*>() == "const rr::core::Order *");
	STATIC_REQUIRE(typeName<std::string>() == "std::basic_string<char>");
	STATIC_REQUIRE(gcpp::detail::typeName<Lambda>().startsWith("(lambda at "));
	STATIC_REQUIRE(typeName<std::vector<std::vector<int>>>() == "std::vector<std::vector<int>>");
#elif defined(__GNUC__)
	STATIC_REQUIRE(typeName<const int&>() == "const int&");
	STATIC_REQUIRE(typeName<const rr::core::Order*>() == "const rr::core::Order*");
	STATIC_REQUIRE(typeName<std::string>() == "std::__cxx11::basic_string<char>");
	STATIC_REQUIRE(gcpp::detail::typeName<Lambda>().endsWith("::<lambda()>"));
	STATIC_REQUIRE(typeName<std::vector<std::vector<int>>>() == "std::vector<std::vector<int> >");
#endif
}
