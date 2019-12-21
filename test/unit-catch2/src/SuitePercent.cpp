//
// Created by gelldur on 22.12.2019.
//
#include <iostream>

#include <catch2/catch.hpp>

#include "gcpp/math/Percent.h"

using namespace gcpp::math;
using namespace Catch::literals;

#include <vector>

TEST_CASE("Should percent", "[math][Percent]")
{
	REQUIRE(Percent<double>{10}.value() == Approx(0.1).epsilon(0.001));
	REQUIRE(Percent<double>{100}.value() == Approx(1).epsilon(0.00001));
	REQUIRE(Percent<double>{1, 3}.value() == Approx(0.333).epsilon(0.01));
}
