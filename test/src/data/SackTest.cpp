//
// Created by Dawid Drozd aka Gelldur on 25.03.18.
//
#include <catch.hpp>

#include <iostream>

#include <data/Sack.h>
#include <data/Currency.h>

TEST_CASE("Should copy assign without anny issues", "[Sack]")
{
	Sack<Currency> sack1;
	{
		sack1.add(Currency{Currency::Type::ADA, 1});
		sack1.add(Currency{Currency::Type::BTC, 1});
		sack1.add(Currency{Currency::Type::ADA, 1});
		sack1.add(Currency{Currency::Type::ETH, 1});
	}
	Sack<Currency> sack2;
	{
		sack2.add(Currency{Currency::Type::ADA, 1});
		sack2.add(Currency{Currency::Type::BTC, 1});
		sack2.add(Currency{Currency::Type::ETH, 1});
	}

	REQUIRE(sack1.get(Currency::Type::ADA).getValue() == 2);
	REQUIRE(sack1.get(Currency::Type::BNB).getValue() == 0);

	REQUIRE(sack2.get(Currency::Type::ADA).getValue() == 1);
	REQUIRE(sack2.get(Currency::Type::BNB).getValue() == 0);

	REQUIRE_NOTHROW(sack2 = sack1);

	REQUIRE(sack1.get(Currency::Type::ADA).getValue() == 2);
	REQUIRE(sack1.get(Currency::Type::BNB).getValue() == 0);

	REQUIRE(sack2.get(Currency::Type::ADA).getValue() == 2);
	REQUIRE(sack2.get(Currency::Type::BNB).getValue() == 0);
}

TEST_CASE("Should copy assign without anny issues When different order", "[Sack]")
{
	Sack<Currency> sack1;
	{
		sack1.add(Currency{Currency::Type::ADA, 1});
		sack1.add(Currency{Currency::Type::BTC, 1});
		sack1.add(Currency{Currency::Type::ADA, 1});
		sack1.add(Currency{Currency::Type::ETH, 1});
	}
	Sack<Currency> sack2;
	{
		sack2.add(Currency{Currency::Type::ADA, 1});
		sack2.add(Currency{Currency::Type::ETH, 1});
	}

	REQUIRE(sack1.get(Currency::Type::ADA).getValue() == 2);
	REQUIRE(sack1.get(Currency::Type::BNB).getValue() == 0);

	REQUIRE(sack2.get(Currency::Type::ADA).getValue() == 1);
	REQUIRE(sack2.get(Currency::Type::BNB).getValue() == 0);

	REQUIRE_NOTHROW(sack2 = sack1);

	REQUIRE(sack1.get(Currency::Type::ADA).getValue() == 2);
	REQUIRE(sack1.get(Currency::Type::BNB).getValue() == 0);

	REQUIRE(sack2.get(Currency::Type::ADA).getValue() == 2);
	REQUIRE(sack2.get(Currency::Type::BNB).getValue() == 0);
}

TEST_CASE("Should copy assign without anny issues When different content", "[Sack]")
{
	Sack<Currency> sack1;
	{
		sack1.add(Currency{Currency::Type::ADA, 1});
	}
	Sack<Currency> sack2;
	{
		sack2.add(Currency{Currency::Type::ETH, 1});
	}

	REQUIRE(sack1.get(Currency::Type::ADA).getValue() == 1);
	REQUIRE(sack2.get(Currency::Type::ADA).getValue() == 0);

	REQUIRE_NOTHROW(sack2 = sack1);

	REQUIRE(sack1.get(Currency::Type::ADA).getValue() == 1);
	REQUIRE(sack2.get(Currency::Type::ADA).getValue() == 1);
}

TEST_CASE("Should move assign without anny issues When different content", "[Sack]")
{
	Sack<Currency> sack1;
	{
		sack1.add(Currency{Currency::Type::ADA, 1});
	}
	Sack<Currency> sack2;
	{
		sack2.add(Currency{Currency::Type::ETH, 1});
	}

	REQUIRE(sack1.get(Currency::Type::ADA).getValue() == 1);
	REQUIRE(sack2.get(Currency::Type::ADA).getValue() == 0);

	REQUIRE_NOTHROW(sack2 = std::move(sack1));

	REQUIRE(sack2.get(Currency::Type::ADA).getValue() == 1);
}
