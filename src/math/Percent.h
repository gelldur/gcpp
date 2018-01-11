//
// Created by Dawid Drozd aka Gelldur on 03.12.17.
//
#pragma once

#include <ostream>
#include <iomanip>

namespace math
{

template<class T = double>
class Percent
{
public:
	//nominator and denominator
	explicit Percent(T nominator, T denominator)
			: _nominator(std::move(nominator))
			, _denominator(std::move(denominator))
	{
		if (_denominator == 0)
		{
			throw std::invalid_argument("Denominator can't be == 0");
		}
	}

	explicit Percent(T percentValue)
			: _nominator(percentValue)
			, _denominator(100.0)
	{
	}

	T value() const
	{
		return (_nominator / _denominator);
	}

	const T& getNominator() const
	{
		return _nominator;
	}

	const T& getDenominator() const
	{
		return _denominator;
	}

	bool isZero() const
	{
		return _nominator == 0;
	}

	bool operator==(const Percent& rhs) const
	{
		if (_denominator == rhs._denominator)
		{
			return _nominator == rhs._nominator;
		}
		else if (_nominator == rhs._nominator)
		{
			return _denominator == rhs._denominator;
		}

		return value() == rhs.value();
	}

	bool operator!=(const Percent& rhs) const
	{
		return !(*this == rhs);
	}

	template<typename O>
	typename std::enable_if<!std::is_same<O, Percent<T>>::value, O>::type
	operator*(const O& other)
	{
		return other * value();
	}

	template<typename O>
	friend typename std::enable_if<!std::is_same<O, Percent<T>>::value, O>::type
	operator*(const O& lhs, const Percent<T>& rhs)
	{
		return lhs * rhs.value();
	}

	template<typename O>
	friend typename std::enable_if<!std::is_same<O, Percent<T>>::value, O>::type
	operator-(const O& lhs, const Percent<T>& rhs)
	{
		if (rhs.isZero())
		{
			return lhs;
		}

		return lhs - ((lhs * rhs._nominator) / rhs._denominator);
	}

	template<typename O>
	friend typename std::enable_if<!std::is_same<O, Percent<T>>::value, O>::type
	operator+(const O& lhs, const Percent<T>& rhs)
	{
		if (rhs.isZero())
		{
			return lhs;
		}
		return lhs + ((lhs * rhs._nominator) / rhs._denominator);
	}

	Percent<T> diff() const
	{
		return Percent<T>{_nominator - _denominator, _denominator};
	}

	friend std::ostream& operator<<(std::ostream& stream, const Percent& percent)
	{
		stream << percent.value() * 100.0 << "%";
		return stream;
	}

private:
	T _nominator;
	T _denominator;
};

}

