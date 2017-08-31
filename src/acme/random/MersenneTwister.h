//
// Created by cezary on 21.09.16.
//
//https://en.wikipedia.org/wiki/Mersenne_Twister - algorithm which we are using
//http://en.cppreference.com/w/cpp/concept/UniformRandomBitGenerator - how to make it work with proper distributions
//only changes I made are to make a class from this algorithm and make it work with proper distribution
#pragma once

#include <limits>
#include <cstdint>

class MersenneTwister
{
public:

	explicit MersenneTwister();
	explicit MersenneTwister(std::uint32_t seed);

	void initialize(std::uint32_t seed);
	unsigned int extract();

	using result_type = std::uint32_t;

	static constexpr result_type min()
	{
		return std::numeric_limits<result_type>::min();
	}

	static constexpr result_type max()
	{
		return std::numeric_limits<result_type>::max();
	}

	result_type operator()()
	{
		return extract();
	}

private:
	std::uint32_t twist();
	std::uint32_t _x[624];
	std::uint32_t _index;
};
