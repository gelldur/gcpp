//
// Created by cezary on 21.09.16.
//
//https://en.wikipedia.org/wiki/Mersenne_Twister - algorithm which we are using
//http://en.cppreference.com/w/cpp/concept/UniformRandomBitGenerator - how to make it work with proper distributions
//only changes I made are to make a class from this algorithm and make it work with proper distribution
#pragma once

#include <limits>

class MersenneTwister
{
public:

	MersenneTwister();
	MersenneTwister(unsigned int seed);

	void initialize(unsigned int seed);
	unsigned int extract();

	using result_type = unsigned int;

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
	unsigned int twist();
	unsigned int _x[624];
	signed int _index;
};
