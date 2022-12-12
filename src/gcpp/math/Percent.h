//
// Created by Dawid Drozd aka Gelldur on 03.12.17.
//
#pragma once

#include <iomanip>
#include <ostream>

namespace gcpp::math
{

template <class T = double>
class [[nodiscard]] Percent
{
public:
	explicit Percent() // by default 0%
		: _nominator{0}
		, _denominator{1}
		, _value(0)
	{ }

	explicit constexpr Percent(const T& nominator, const T& denominator)
		: _nominator(nominator)
		, _denominator(denominator)
		, _value((_nominator / _denominator))
	{
		if(_denominator == 0)
		{
			throw std::invalid_argument("Denominator can't be == 0");
		}
	}

	explicit constexpr Percent(T&& nominator, T&& denominator)
		: _nominator(std::forward<T>(nominator))
		, _denominator(std::forward<T>(denominator))
		, _value((_nominator / _denominator))
	{
		if(_denominator == 0)
		{
			throw std::invalid_argument("Denominator can't be == 0");
		}
	}

	explicit constexpr Percent(T&& percentValue)
		: _nominator(std::forward<T>(percentValue))
		, _denominator(100.0)
		, _value((_nominator / _denominator))
	{ }

	Percent(const Percent& other)
		: _nominator(other._nominator)
		, _denominator(other._denominator)
		, _value(other._value)
	{ }

	Percent(Percent&& other) noexcept
		: _nominator(std::move(other._nominator))
		, _denominator(std::move(other._denominator))
		, _value(std::move(other._value))
	{ }

	template <typename E>
	explicit Percent(const Percent<E>& other)
		: _nominator(other.getNominator())
		, _denominator(other.getDenominator())
		, _value((_nominator / _denominator))
	{ }

	Percent& operator=(const Percent& other)
	{
		_nominator = (other._nominator);
		_denominator = (other._denominator);
		_value = (other._value);
		return *this;
	}

	Percent& operator=(Percent&& other) noexcept
	{
		_nominator = std::move(other._nominator);
		_denominator = std::move(other._denominator);
		_value = std::move(other._value);
		return *this;
	}

	template <typename E>
	Percent& operator=(const Percent<E>& other) noexcept
	{
		_nominator = other.getNominator();
		_denominator = other.getDenominator();
		_value = (_nominator / _denominator);
		return *this;
	}

	[[nodiscard]] const T& value() const
	{
		return _value;
	}

	[[nodiscard]] T percentValue() const
	{
		return value() * 100;
	}

	[[nodiscard]] const T& getNominator() const
	{
		return _nominator;
	}

	[[nodiscard]] const T& getDenominator() const
	{
		return _denominator;
	}

	[[nodiscard]] bool isZero() const
	{
		return _nominator == 0;
	}

	bool operator==(const Percent& rhs) const
	{
		if(_denominator == rhs._denominator)
		{
			return _nominator == rhs._nominator;
		}

		return value() == rhs.value();
	}

	bool operator!=(const Percent& rhs) const
	{
		return !(*this == rhs);
	}

	bool operator<(const Percent& rhs) const
	{
		return value() < rhs.value();
	}

	bool operator>(const Percent& rhs) const
	{
		return rhs < *this;
	}

	bool operator<=(const Percent& rhs) const
	{
		return !(rhs < *this);
	}

	bool operator>=(const Percent& rhs) const
	{
		return !(*this < rhs);
	}

	template <typename O>
	typename std::enable_if<!std::is_same<O, Percent<T>>::value, O>::type operator*(const O& other)
	{
		return other * value();
	}

	template <typename O>
	friend typename std::enable_if<!std::is_same<O, Percent<T>>::value, O>::type operator*(
		const O& lhs, const Percent<T>& rhs)
	{
		return lhs * rhs.value();
	}

	template <typename O>
	friend typename std::enable_if<!std::is_same<O, Percent<T>>::value, O>::type operator-(
		const O& lhs, const Percent<T>& rhs)
	{
		if(rhs.isZero())
		{
			return lhs;
		}

		return lhs - ((lhs * rhs._nominator) / rhs._denominator);
	}

	template <typename O>
	friend typename std::enable_if<!std::is_same<O, Percent<T>>::value, O>::type operator+(
		const O& lhs, const Percent<T>& rhs)
	{
		if(rhs.isZero())
		{
			return lhs;
		}
		return lhs + ((lhs * rhs._nominator) / rhs._denominator);
	}

	Percent<T> operator+(const Percent<T>& rhs) const
	{
		return Percent<T>{(value() + rhs.value()) * 100};
	}

	Percent<T> operator-(const Percent<T>& rhs) const
	{
		return Percent<T>{(value() - rhs.value()) * 100};
	}

	Percent operator*(const Percent<T>& rhs) const
	{
		return Percent{_nominator * rhs._nominator, _denominator * rhs._denominator};
	}

	Percent<T> operator/(const Percent<T>& rhs) const
	{
		return Percent<T>{_nominator * rhs._denominator, _denominator * rhs._nominator};
	}

	template <typename O>
	O operator*(const O& rhs) const
	{
		return value() * rhs;
	}

	Percent<T> operator-() const
	{
		return Percent<T>{-_nominator, _denominator};
	}

	Percent<T> diff() const
	{
		return Percent<T>{_nominator - _denominator, _denominator};
	}

	friend std::ostream& operator<<(std::ostream& stream, const Percent& percent)
	{
		std::stringstream localStream;
		localStream << percent.percentValue() << '%';
		stream << localStream.str();
		return stream;
	}

private:
	T _nominator;
	T _denominator;
	T _value; // Fast cache
};

template <typename T, typename O>
inline Percent<T> multiply(const O& lhs, const Percent<T>& rhs)
{
	return Percent<T>{lhs * rhs.getNominator(), rhs.getDenominator()};
}

inline namespace percent_literals
{

Percent<long double> operator"" _bps(long double);
Percent<long double> operator"" _bps(unsigned long long int);

} // namespace percent_literals

} // namespace gcpp::math
