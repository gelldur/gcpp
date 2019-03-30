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

TEST_CASE("Where", "Simple test")
{
	exampleUseCaseWhere(10);
}
