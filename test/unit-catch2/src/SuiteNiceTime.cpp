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
