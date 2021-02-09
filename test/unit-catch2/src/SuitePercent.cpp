//
// Created by gelldur on 22.12.2019.
//
#include <iostream>

#include <catch2/catch.hpp>

#include "gcpp/math/Percent.h"
#include "gcpp/nice/to_string.hpp"

using namespace gcpp::math;
using namespace Catch::literals;

TEST_CASE("Should percent", "[math][Percent]")
{
	REQUIRE(Percent<double>{10}.value() == Approx(0.1).epsilon(0.001));
	REQUIRE(Percent<double>{100}.value() == Approx(1).epsilon(0.00001));
	REQUIRE(Percent<double>{1, 3}.value() == Approx(0.333).epsilon(0.01));
}

TEST_CASE("Should format nice", "[math][Percent]")
{
	REQUIRE(gcpp::nice::toString(Percent<double>{10}) == "10%");

	{
		std::stringstream stream;
		stream << std::setw(8);
		stream << Percent<double>{1};
		REQUIRE(stream.str() == "      1%");
		REQUIRE(stream.str().size() == 8);
	}
	{
		std::stringstream stream;
		stream << std::setw(8) << std::left;
		stream << Percent<double>{1};
		REQUIRE(stream.str() == "1%      ");
		REQUIRE(stream.str().size() == 8);
	}
}

TEST_CASE("Multiply by percent", "[math][Percent]")
{
	SECTION("Same underlying type")
	{
		const Percent<double> tenPercent{10};
		const double value = 2;
		REQUIRE(value * tenPercent == Approx(0.2).epsilon(0.001));
		REQUIRE(value * tenPercent == tenPercent * value);
	}
	SECTION("Different underlying type")
	{
		const Percent<double> tenPercent{10};
		const int value = 2;
		REQUIRE(value * tenPercent == 0);
		REQUIRE(value * tenPercent == tenPercent * value);
	};
}
