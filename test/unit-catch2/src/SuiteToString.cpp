//
// Created by gelldur on 04.01.2020.
//
#include <iostream>

#include <catch2/catch.hpp>

#include "gcpp/string/to_string.hpp"

TEST_CASE("Should format time as ISO8601", "[ToString]")
{
	// 1578143217284824460 -> 2020-01-04 13:06:57
	const std::chrono::system_clock::time_point timePoint{
		std::chrono::nanoseconds{1578143217284824460ULL}};

	REQUIRE(gcpp::string::to_string::formatTimeISO8601(timePoint) == "2020-01-04 13:06:57");
}
