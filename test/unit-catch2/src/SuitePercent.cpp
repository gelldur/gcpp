//
// Created by gelldur on 22.12.2019.
//
#include <iomanip>
#include <iostream>
#include <sstream>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "gcpp/math/Percent.h"
#include "gcpp/nice/to_string.hpp"

using namespace gcpp::math;
using namespace Catch;
using gcpp::math::Percent;

TEST_CASE("Should percent", "[math][Percent]")
{
	REQUIRE(Percent<double>{10}.value() == Approx(0.1).epsilon(0.001));
	REQUIRE(Percent<double>{100}.value() == Approx(1).epsilon(0.00001));
	REQUIRE(Percent<double>{1, 3}.value() == Approx(0.333).epsilon(0.01));

	REQUIRE(1_bps == Percent<long double>{0.01L});
	REQUIRE(100_bps == Percent<long double>{1});

	REQUIRE((1_bps).value() == Approx(0.0001).epsilon(0.01));
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

		stream.str("");
		stream << std::setw(10);
		stream << Percent<double>{11.123};
		REQUIRE(stream.str() == "   11.123%");
		REQUIRE(stream.str().size() == 10);
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
	}

	SECTION("Different underlying type")
	{
		const Percent<double> tenPercent{10};
		const int value = 2;
		REQUIRE(value * tenPercent == 0);
	}

	SECTION("Transitivity")
	{
		REQUIRE(2.F * Percent<double>{0.1} == Approx(0.002).epsilon(0.0001));
		REQUIRE(Percent<double>{0.1} * 2.F == Approx(0.002).epsilon(0.0001));
	}
}

TEST_CASE("Percent * Percent", "[math][Percent]")
{
	SECTION("Same underlying type")
	{
		const Percent<double> a{10};
		const Percent<double> b{65};
		REQUIRE((a * b).value() == Approx(0.065).epsilon(0.001));
		REQUIRE((a * b).percentValue() == Approx(6.5).epsilon(0.001));
		REQUIRE(b * a == a * b);
	}
	// TODO implement
	//	SECTION("Different underlying type")
	//	{
	//		const Percent<double> a{10};
	//		const Percent<int> b{65};
	//		REQUIRE(b * a == a * b);
	//		REQUIRE((a * b).value() == ?);
	//	};
}

TEST_CASE("Percent / Percent", "[math][Percent]")
{
	SECTION("Same underlying type")
	{
		const Percent<double> a{10};
		const Percent<double> b{65};
		REQUIRE((a / b).value() == Approx(0.1538).epsilon(0.001));
	}
	// TODO implement
	//	SECTION("Different underlying type")
	//	{
	//		const Percent<double> a{10};
	//		const Percent<int> b{65};
	//		REQUIRE((a / b).value() == ?);
	//	};
}

TEST_CASE("Percent * scalar", "[math][Percent]")
{
	const Percent<double> a{0.02}; // 0.02%
	auto result = multiply(10, a);
	static_assert(std::is_same_v<decltype(result), Percent<double>>, "Expected same type");
	CHECK(result.percentValue() == Approx(0.2).epsilon(0.001));
}

namespace
{

template <class T>
std::string streamed(const T& value, auto&&... manipulators)
{
	std::ostringstream stream;
	(stream << ... << manipulators) << value;
	return stream.str();
}

} // namespace

TEST_CASE("Percent construction", "[Percent]")
{
	SECTION("constexpr friendly")
	{
		static_assert(Percent<double>{12.5}.value() == 0.125);
		static_assert(Percent<double>{1.0, 4.0}.percentValue() == 25.0);
		static_assert(Percent<double>{}.isZero());
	}

	SECTION("lvalue arguments are accepted")
	{
		double percentValue = 12.1;
		const double nominator = 1.0;
		REQUIRE(Percent{percentValue}.percentValue() == 12.1);
		REQUIRE(Percent{nominator, 4.0}.value() == 0.25);
	}

	SECTION("zero denominator throws before dividing (no SIGFPE for integers)")
	{
		REQUIRE_THROWS_AS((Percent<int>{1, 0}), std::invalid_argument);
		REQUIRE_THROWS_AS((Percent<double>{1.0, 0.0}), std::invalid_argument);
		REQUIRE_THROWS_AS((Percent<int>{Percent<double>{1.0, 0.4}}), std::invalid_argument);
	}

	SECTION("default constructor works for integral T")
	{
		REQUIRE(Percent<int>{}.getDenominator() == 100);
	}

	SECTION("ZeroSafe")
	{
		REQUIRE(Percent<double>::ZeroSafe(1.0, 0.0).isZero());
		REQUIRE(Percent<double>::ZeroSafe(1.0, 2.0).percentValue() == 50.0);
	}
}

TEST_CASE("Percent arithmetic", "[Percent]")
{
	SECTION("percent * amount is commutative and does not depend on constness")
	{
		Percent mutableFee{2.0};
		const Percent constFee{2.0};
		REQUIRE(mutableFee * 150.0 == 3.0);
		REQUIRE(constFee * 150.0 == 3.0);
		REQUIRE(150.0 * constFee == 3.0);
		STATIC_REQUIRE(std::same_as<decltype(constFee * 150.0), double>);
	}

	SECTION("amount +/- percent")
	{
		REQUIRE(150.0 + Percent{2.0} == 153.0);
		REQUIRE(150.0 - Percent{2.0} == 147.0);
		REQUIRE(150.0 - Percent<double>{} == 150.0);
	}

	SECTION("percent +/- percent stays exact")
	{
		REQUIRE(std::format("{}", Percent{12.1} + Percent{1.0}) == "13.1%");
		REQUIRE(std::format("{}", Percent{12.1} - Percent{1.0}) == "11.1%");
		REQUIRE((Percent{1.0, 4.0} + Percent{1.0, 2.0}) == Percent{75.0});
	}

	SECTION("percent * percent and / percent")
	{
		REQUIRE((Percent{10.0} * Percent{10.0}) == Percent{1.0});
		REQUIRE((Percent{50.0} / Percent{25.0}) == Percent{200.0});
		REQUIRE_THROWS_AS(Percent{50.0} / Percent<double>{}, std::invalid_argument);
	}

	SECTION("unary minus, diff, multiply")
	{
		REQUIRE(-Percent{5.0} == Percent{-5.0});
		REQUIRE(Percent{110.0}.diff() == Percent{10.0});
		REQUIRE(gcpp::math::multiply(2.0, Percent{10.0}) == Percent{20.0});
	}

	SECTION("comparisons across denominators")
	{
		REQUIRE(Percent{1.0, 4.0} == Percent{25.0});
		REQUIRE(Percent{1.0, 4.0} != Percent{26.0});
		REQUIRE(Percent{1.0} < Percent{2.0});
		REQUIRE(Percent{2.0} >= Percent{2.0});
	}
}

TEST_CASE("Percent operator<<", "[Percent]")
{
	const Percent p{12.1};

	REQUIRE(streamed(p) == "12.1%");
	REQUIRE(streamed(p, std::left, std::setw(11)) == "12.1%      ");
	REQUIRE(streamed(p, std::setw(8)) == "   12.1%");
	REQUIRE(streamed(p, std::setfill('*'), std::right, std::setw(8)) == "***12.1%");
	REQUIRE(streamed(p, std::fixed, std::setprecision(2)) == "12.10%");
	REQUIRE(streamed(p, std::showpos) == "+12.1%");
	REQUIRE(streamed(Percent{1.0, 3.0}) == "33.3333%");

	SECTION("width is consumed by the percent only")
	{
		std::ostringstream stream;
		stream << std::left << std::setw(8) << p << '|' << p;
		REQUIRE(stream.str() == "12.1%   |12.1%");
	}

	SECTION("wide streams")
	{
		std::wostringstream stream;
		stream << std::left << std::setw(7) << p << L'|';
		REQUIRE(stream.str() == L"12.1%  |");
	}
}

TEST_CASE("Percent std::format", "[Percent]")
{
	const Percent p{12.1};

	SECTION("defaults")
	{
		REQUIRE(std::format("{}", p) == "12.1%");
		REQUIRE(std::format("{}", Percent{1.0, 3.0}) == "33.333333333333336%");
		REQUIRE(std::format("{}", Percent<float>{12.5f}) == "12.5%");
		REQUIRE(std::format(L"{:.1f}", p) == L"12.1%");
	}

	SECTION("whole spec belongs to std::formatter<T>, '%' is appended after it")
	{
		REQUIRE(std::format("{:.2f}", p) == "12.10%");
		REQUIRE(std::format("{:+.1f}", p) == "+12.1%");
		REQUIRE(std::format("{:.3e}", p) == "1.210e+01%");
		REQUIRE(std::format("{:08.2f}", p) == "00012.10%");
		REQUIRE(std::format("{:>8}", p) == "    12.1%");
		REQUIRE(std::format("{:<8}|", p) == "12.1    %|");
		REQUIRE(std::format("{:<{}}|", p, 8) == "12.1    %|");
	}

	SECTION("invalid specs for T are rejected")
	{
		const auto runtimeFormat = [&p](std::string_view spec) {
			return std::vformat(spec, std::make_format_args(p));
		};
		REQUIRE_THROWS_AS(runtimeFormat("{:d}"), std::format_error);
		REQUIRE_THROWS_AS(runtimeFormat("{:<x}"), std::format_error);
	}
}
