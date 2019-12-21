//
// Created by Dawid Drozd aka Gelldur on 25.03.18.
//
#include <iostream>

#include <catch2/catch.hpp>

#include "gcpp/container/Sack.h"

using gcpp::container::Sack;

namespace gcpp::test
{

class Currency
{
public:
	enum class Type
	{
		None,
		ADA,
		BTC,
		ETH,
		BNB
	};

	Currency(Type type, int value)
		: _value(value)
		, _type(type)
	{}

	Currency(Type type)
		: _value(0)
		, _type(type)
	{}

	[[nodiscard]] int getValue() const
	{
		return _value;
	}

	[[nodiscard]] Type getType() const
	{
		return _type;
	}

	Currency& operator+=(const Currency& other)
	{
		assert(_type == other._type);
		_value += other.getValue();
		return *this;
	}

private:
	int _value = 0;
	Type _type = Type::None;
};

TEST_CASE("Should copy assign without anny issues", "[Sack]")
{
	gcpp::container::Sack<Currency> sack1;
	{
		sack1.add(Currency{Currency::Type::ADA, 1});
		sack1.add(Currency{Currency::Type::BTC, 1});
		sack1.add(Currency{Currency::Type::ADA, 1});
		sack1.add(Currency{Currency::Type::ETH, 1});
	}
	gcpp::container::Sack<Currency> sack2;
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
	gcpp::container::Sack<Currency> sack1;
	{
		sack1.add(Currency{Currency::Type::ADA, 1});
		sack1.add(Currency{Currency::Type::BTC, 1});
		sack1.add(Currency{Currency::Type::ADA, 1});
		sack1.add(Currency{Currency::Type::ETH, 1});
	}
	gcpp::container::Sack<Currency> sack2;
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
	gcpp::container::Sack<Currency> sack1;
	{
		sack1.add(Currency{Currency::Type::ADA, 1});
	}
	gcpp::container::Sack<Currency> sack2;
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
	gcpp::container::Sack<Currency> sack1;
	{
		sack1.add(Currency{Currency::Type::ADA, 1});
	}
	gcpp::container::Sack<Currency> sack2;
	{
		sack2.add(Currency{Currency::Type::ETH, 1});
	}

	REQUIRE(sack1.get(Currency::Type::ADA).getValue() == 1);
	REQUIRE(sack2.get(Currency::Type::ADA).getValue() == 0);

	REQUIRE_NOTHROW(sack2 = std::move(sack1));

	REQUIRE(sack2.get(Currency::Type::ADA).getValue() == 1);
}

} // namespace gcpp::test
