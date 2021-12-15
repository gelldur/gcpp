//
// Created by gelldur on 14/12/2021.
//
#include "Percent.h"

namespace gcpp::math
{

inline namespace percent_literals
{

Percent<long double> operator""_bps(long double value)
{
	return Percent<long double>(value, 10000.0);
}

Percent<long double> operator""_bps(unsigned long long int value)
{
	return Percent<long double>(static_cast<long double>(value), 10000);
}

} // namespace percent_literals

} // namespace gcpp::math
