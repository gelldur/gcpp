//
// Created by cezary on 21.09.16.
//

#include "MersenneTwister.h"

MersenneTwister::MersenneTwister()
{
	initialize(10);
}

MersenneTwister::MersenneTwister(std::uint32_t seed)
{
	initialize(seed);
}

void MersenneTwister::initialize(std::uint32_t seed)
{
	_index = 624;
	std::uint32_t i = 1;
	*_x = seed;
	std::uint32_t* j = _x;
	std::uint32_t _a;
	unsigned _b;
	do
	{
		_a = i + 1812433253 * (*j ^ (*j >> 30));
		*(j + 1) = _a;
		_b = i + 1812433253 * (_a ^ (_a >> 30)) + 1;
		i += 2;
		*(j + 2) = _b;
		j += 2;
	} while (j < _x + 0x26C);
}

std::uint32_t MersenneTwister::twist()
{
	std::uint32_t top = 397, l = 623;
	std::uint32_t* j = _x;
	std::uint32_t i;
	std::uint32_t _c, out;
	signed int _f;
	do
	{
		i = (top - 396) % 624;
		_c = (*j ^ ((*j ^ (_x[i])) & 0x7FFFFFFF)) >> 1;
		if ((*j ^ (*j ^ _x[i])) & 1)
		{
			_c ^= 0x9908B0DFu;
		}
		_f = top++;
		out = _c ^ _x[_f % 624];
		*j = out;
		++j;
		--l;
	} while (l);
	_index = 0;
	return out;
}

std::uint32_t MersenneTwister::extract()
{
	unsigned i = _index;
	if (_index >= 624)
	{
		twist();
		i = _index;
	}
	std::uint32_t e = _x[i];
	std::uint32_t _v = _x[i] >> 11;
	_index = i + 1;
	std::uint32_t def = (((_v ^ e) & 0xFF3A58AD) << 7) ^_v ^e;
	return ((def & 0xFFFFDF8C) << 15) ^ def ^ ((((def & 0xFFFFDF8C) << 15) ^ def) >> 18);
}
