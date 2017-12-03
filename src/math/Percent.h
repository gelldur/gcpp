//
// Created by Dawid Drozd aka Gelldur on 03.12.17.
//
#pragma once

#include <ostream>
#include <iomanip>

namespace math
{

template<class T>
class Percent
{
public:
	explicit Percent(T value, T hundredPercent)
			: _value(std::move(value))
			, _hundredPercent(std::move(hundredPercent))
	{
	}

	explicit Percent(T percentValue)
			: _value(percentValue / 100.0)
			, _hundredPercent(100.0)
	{
	}

	const T& getValue() const
	{
		return _value;
	}

	const T& getHundredPercent() const
	{
		return _hundredPercent;
	}

	template<typename O>
	typename std::enable_if<!std::is_same<O, Percent<T>>::value, O>::type
	operator*(const O& other)
	{
		return other * _value;
	}

	template<typename O>
	friend typename std::enable_if<!std::is_same<O, Percent<T>>::value, O>::type
	operator*(const O& lhs, const Percent<T>& rhs)
	{
		return lhs * rhs._value;
	}

	template<typename O>
	friend typename std::enable_if<!std::is_same<O, Percent<T>>::value, O>::type
	operator-(const O& lhs, const Percent<T>& rhs)
	{
		return lhs - (lhs * rhs._value);
	}

	template<typename O>
	friend typename std::enable_if<!std::is_same<O, Percent<T>>::value, O>::type
	operator+(const O& lhs, const Percent<T>& rhs)
	{
		return lhs + (lhs * rhs._value);
	}

	Percent<T> diff() const
	{
		return Percent<T>{_value - _hundredPercent, _hundredPercent};
	}

	friend std::ostream& operator<<(std::ostream& stream, const Percent& percent)
	{
		stream << (percent._value / percent._hundredPercent) * 100.0 << "%";
		return stream;
	}

private:
	T _value;
	T _hundredPercent;
};

}

