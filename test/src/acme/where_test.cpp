//
// Created by gelldur on 30.03.19.
//
#include <iostream>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "acme/where.hpp"

namespace
{

void exampleUseCaseWhere(int argument)
{
	// Example output
	// Called: void (anonymous namespace)::exampleUseCaseWhere(int):18
	std::cout << "Called: " << WHERE << std::endl;
	std::string where = WHERE; // copy c string
	std::cout << where << std::endl;

	constexpr auto where2 = WHERE_STR;
	std::string_view where_view{where2.get(), where2.length()};
	std::cout << where_view << std::endl;
}

} // namespace

TEST_CASE("Where usage example", "[where][constexpr_string]")
{
	exampleUseCaseWhere(10);
}

TEST_CASE("Should work at compile time", "[constexpr_string]")
{
	constexpr auto foo = string::make("foo");
	constexpr auto bar = string::make("bar");

	constexpr auto foobar = foo + bar;

	static_assert(foobar == "foobar");
	static_assert("foobar" == foobar);

	static_assert((foo + "bar") == "foobar");
	static_assert("foobar" == ("foo" + bar));

	static_assert((foo + bar)[3] == 'b');
	static_assert((foo + bar).length() == 6);
	REQUIRE(foo == "foo");
	REQUIRE(bar == "bar");
	REQUIRE(foobar != "bar");
	REQUIRE(foobar != "foo");
	REQUIRE(foobar == "foobar");
}
