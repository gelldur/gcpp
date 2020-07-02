//
// Created by gelldur on 04.01.2020.
//
#include <catch2/catch.hpp>

#include "gcpp/string/from_string.hpp"
#include "gcpp/string/to_string.hpp"

using namespace std::chrono_literals;

#include "gcpp/nice/time.hpp"

TEST_CASE("Should parse time as ISO8601", "[FromString]")
{
	// 1578143217000000000 -> 2020-01-04 13:06:57
	const std::chrono::system_clock::time_point timePoint{
		std::chrono::nanoseconds{1578143217000000000ULL}};
	constexpr auto DATE = "2020-01-04 13:06:57";
	REQUIRE(gcpp::string::from_string::parseTimeISO8601(DATE).has_value());

	CAPTURE(gcpp::string::from_string::parseTimeISO8601(DATE)->time_since_epoch().count());
	REQUIRE(gcpp::string::from_string::parseTimeISO8601(DATE) == timePoint);
}

TEST_CASE("Should parse time as ISO8601 back and forth When using to_string and from_string",
		  "[FromString][ToString]")
{
	// 1578143217000000000 -> 2020-01-04 13:06:57
	const std::chrono::system_clock::time_point trimmedTimePoint{
		std::chrono::nanoseconds{1578143217000000000ULL}};

	std::chrono::system_clock::time_point timePoint{
		std::chrono::nanoseconds{1578143217284824499ULL}};

	for(int i = 0; i < 11; ++i)
	{
		const auto toString = gcpp::string::to_string::formatTimeISO8601(timePoint);
		REQUIRE(toString == "2020-01-04 13:06:57");
		timePoint = gcpp::string::from_string::parseTimeISO8601(toString).value();
		REQUIRE(timePoint == trimmedTimePoint);
	}
}

TEST_CASE("Should fail to parse time as ISO8601 When data is malformed", "[FromString]")
{
	SECTION("Invalid date")
	{
		REQUIRE(gcpp::string::from_string::parseTimeISO8601("XXXXXXXX").has_value() == false);
	}
	SECTION("Empty date")
	{
		REQUIRE(gcpp::string::from_string::parseTimeISO8601("").has_value() == false);
	}
	SECTION("Day is invalid")
	{
		const auto test = "2020-01-00 13:06:57";
		REQUIRE(gcpp::string::from_string::parseTimeISO8601(test).has_value() == false);
	}
	SECTION("Month is invalid")
	{
		const auto test = "2020-13-31 13:06:57";
		REQUIRE(gcpp::string::from_string::parseTimeISO8601(test).has_value() == false);
	}
	SECTION("Time is invalid")
	{
		const auto test = "2020-13-31 33:06:57";
		REQUIRE(gcpp::string::from_string::parseTimeISO8601(test).has_value() == false);
	}
}

TEST_CASE("Should parse time as ISO8601 with fraction", "[FromString]")
{
	USING_NICE_TIME;
	// 1578143217000000000 -> 2020-01-04 13:06:57
	SECTION("Format: 2020-01-04 13:06:57.344Z")
	{
		const std::chrono::system_clock::time_point timePoint{
			std::chrono::nanoseconds{1578143217344000000ULL}};
		constexpr auto DATE = "2020-01-04 13:06:57.344Z";
		REQUIRE(gcpp::string::from_string::parseTimeISO8601WithFraction(DATE).has_value());

		CAPTURE(gcpp::string::from_string::parseTimeISO8601WithFraction(DATE)
					->time_since_epoch()
					.count());
		REQUIRE(gcpp::string::from_string::parseTimeISO8601WithFraction(DATE) == timePoint);
	}
	SECTION("Format: 2020-01-04T13:06:57.344Z (with 'T')")
	{
		const std::chrono::system_clock::time_point timePoint{
			std::chrono::nanoseconds{1578143217344000000ULL}};
		constexpr auto DATE = "2020-01-04T13:06:57.344Z";
		REQUIRE(gcpp::string::from_string::parseTimeISO8601WithFraction(DATE).has_value());

		CAPTURE(gcpp::string::from_string::parseTimeISO8601WithFraction(DATE)
					->time_since_epoch()
					.count());
		REQUIRE(gcpp::string::from_string::parseTimeISO8601WithFraction(DATE) == timePoint);
	}
	SECTION("Format: 2020-01-04 13:06:57.000Z parse with fraction")
	{
		const std::chrono::system_clock::time_point timePoint{
			std::chrono::nanoseconds{1578143217000000000ULL}};
		constexpr auto DATE = "2020-01-04 13:06:57.000Z";

		const auto result = gcpp::string::from_string::parseTimeISO8601(DATE);
		REQUIRE(result.has_value());
		CAPTURE(result->time_since_epoch().count());
		REQUIRE(result == timePoint);
	}
}

TEST_CASE("Should parse time When using fraction or not", "[FromString]")
{
	SECTION("Format: 2020-01-04 13:06:57.344Z parse with NO fraction")
	{
		const std::chrono::system_clock::time_point timePoint{
			std::chrono::nanoseconds{1578143217344000000ULL}};
		constexpr auto DATE = "2020-01-04 13:06:57.344Z";

		const auto result = gcpp::string::from_string::parseTimeISO8601(DATE);
		REQUIRE(result.has_value());
		CAPTURE(result->time_since_epoch().count());
		REQUIRE(result == (timePoint - 344ms)); // NO ms
	}
	SECTION("Format: 2020-01-04 13:06:57 parse with fraction")
	{
		const std::chrono::system_clock::time_point timePoint{
			std::chrono::nanoseconds{1578143217000000000ULL}};
		constexpr auto DATE = "2020-01-04 13:06:57";

		const auto result = gcpp::string::from_string::parseTimeISO8601WithFraction(DATE);
		REQUIRE(result.has_value());
		CAPTURE(result->time_since_epoch().count());
		REQUIRE(result == timePoint);
	}
}
