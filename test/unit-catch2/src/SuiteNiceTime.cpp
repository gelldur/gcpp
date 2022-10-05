//
// Created by gelldur on 21.12.2019.
//
#include <iomanip>
#include <sstream>

#include <catch2/catch.hpp>

#include "gcpp/nice/time.hpp"
#include "gcpp/nice/to_string.hpp"
#include "gcpp/string/from_string.hpp"

using namespace std::chrono;

USING_NICE_TIME

TEST_CASE("Should format as desired", "[NiceTime]")
{
	std::stringstream stream;
	stream << 12s;
	REQUIRE(stream.str() == "12s");
	stream.str("");

	stream << 120ms;
	REQUIRE(stream.str() == "120ms");

	stream.str("");
	stream << gcpp::nice::time::human{25h};
	REQUIRE(stream.str() == "1d 1h");

	REQUIRE(gcpp::nice::toString(gcpp::nice::time::human{25h}) == "1d 1h");
	REQUIRE(gcpp::nice::toString(gcpp::nice::time::human{24h}) == "1d");
	REQUIRE(gcpp::nice::toString(gcpp::nice::time::human{23h}) == "23h");
	REQUIRE(gcpp::nice::toString(gcpp::nice::time::human{3min}) == "3m");
	REQUIRE(gcpp::nice::toString(gcpp::nice::time::human{3s}) == "3s");
	REQUIRE(gcpp::nice::toString(gcpp::nice::time::human{3ms}) == "3ms");
	REQUIRE(gcpp::nice::toString(gcpp::nice::time::human{3us}) == "3µs");
	REQUIRE(gcpp::nice::toString(gcpp::nice::time::human{1us}).size() == 4);
	REQUIRE(gcpp::nice::toString(gcpp::nice::time::human{3ns}) == "3ns");
	REQUIRE(gcpp::nice::toString(gcpp::nice::time::human{6ns + 5us + 4ms + 3s + 2min + 1h}) ==
			"1h 2m 3s 4ms 5µs 6ns");
}

TEST_CASE("Should format as UTC time point", "[NiceTime]")
{
	const auto timePoint = gcpp::string::from_string::parseTimeISO8601("2020-01-04 10:43:44");
	REQUIRE(timePoint.has_value());

	std::stringstream stream;
	stream << timePoint.value();

	REQUIRE(stream.str() == "2020-01-04 10:43:44");
}

TEST_CASE("Should nice format as ISO8601", "[NiceTime]")
{
	auto timePoint = gcpp::string::from_string::parseTimeISO8601("2020-01-04 10:43:44");
	REQUIRE(timePoint.has_value());

	std::stringstream stream;
	SECTION("Default without milliseconds")
	{
		timePoint.value() += 321ms;
		stream << gcpp::nice::time::asISO8601{timePoint.value()};
		REQUIRE(stream.str() == "2020-01-04 10:43:44");
	}
	SECTION("With milliseconds")
	{
		timePoint.value() += 321ms;
		stream << gcpp::nice::time::asISO8601{timePoint.value(), true};
		REQUIRE(stream.str() == "2020-01-04 10:43:44.321");
	}
	SECTION("With milliseconds and leading 0")
	{
		timePoint.value() += 21ms;
		stream << gcpp::nice::time::asISO8601{timePoint.value(), true};
		REQUIRE(stream.str() == "2020-01-04 10:43:44.021");
	}
	SECTION("With milliseconds and leading 00")
	{
		timePoint.value() += 1ms;
		stream << gcpp::nice::time::asISO8601{timePoint.value(), true};
		REQUIRE(stream.str() == "2020-01-04 10:43:44.001");
	}
	SECTION("With milliseconds and fill with 0")
	{
		stream << gcpp::nice::time::asISO8601{timePoint.value(), true};
		REQUIRE(stream.str() == "2020-01-04 10:43:44.000");
	}
	SECTION("Check for malformed format")
	{
		stream << gcpp::nice::time::asISO8601{timePoint.value(), true};
		REQUIRE(stream.str() == "2020-01-04 10:43:44.000");
		stream.str("");
		stream << std::setw(3) << 21; // we shouldn't have leading 0
		CHECK(stream.str() == " 21");
	}
}

namespace my::other::name::space
{

void testInNameSpace()
{
	std::stringstream stream;
	stream << 1h;
	REQUIRE(stream.str() == "1h");
}

} // namespace my::other::name::space

TEST_CASE("Should format as desired #2", "[NiceTime]")
{
	my::other::name::space::testInNameSpace();
}
