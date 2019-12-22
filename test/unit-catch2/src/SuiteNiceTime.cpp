//
// Created by gelldur on 21.12.2019.
//
#include <sstream>

#include <catch2/catch.hpp>

#include "gcpp/nice/time.hpp"

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

	REQUIRE(gcpp::toString(gcpp::nice::time::human{25h}) == "1d 1h");
	REQUIRE(gcpp::toString(gcpp::nice::time::human{24h}) == "1d");
	REQUIRE(gcpp::toString(gcpp::nice::time::human{23h}) == "23h");
	REQUIRE(gcpp::toString(gcpp::nice::time::human{3min}) == "3m");
	REQUIRE(gcpp::toString(gcpp::nice::time::human{3s}) == "3s");
	REQUIRE(gcpp::toString(gcpp::nice::time::human{3ms}) == "3ms");
	REQUIRE(gcpp::toString(gcpp::nice::time::human{3us}) == "3µs");
	REQUIRE(gcpp::toString(gcpp::nice::time::human{1us}).size() == 4);
	REQUIRE(gcpp::toString(gcpp::nice::time::human{3ns}) == "3ns");
	REQUIRE(gcpp::toString(gcpp::nice::time::human{6ns + 5us + 4ms + 3s + 2min + 1h}) ==
			"1h 2m 3s 4ms 5µs 6ns");
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
