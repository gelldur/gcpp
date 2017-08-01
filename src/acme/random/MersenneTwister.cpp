//
// Created by cezary on 21.09.16.
//

#include "MersenneTwister.h"

MersenneTwister::MersenneTwister()
{
	initialize(10);
}

MersenneTwister::MersenneTwister(unsigned int seed)
{
	initialize(seed);
}

void MersenneTwister::initialize(unsigned int seed)
{
	_index = 624;
	signed int i = 1;
	*_x = seed;
	unsigned int* j = _x;
	unsigned int _a;
	int _b;
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

unsigned int MersenneTwister::twist()
{
	signed int top = 397, l = 623;
	unsigned int* j = _x;
	int i;
	unsigned int _c, out;
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

unsigned int MersenneTwister::extract()
{
	int i = _index;
	if (_index >= 624)
	{
		twist();
		i = _index;
	}
	unsigned int e = _x[i];
	unsigned int _v = _x[i] >> 11;
	_index = i + 1;
	int def = (((_v ^ e) & 0xFF3A58AD) << 7) ^_v ^e;
	return ((def & 0xFFFFDF8C) << 15) ^ def ^ ((((def & 0xFFFFDF8C) << 15) ^ def) >> 18);
}
