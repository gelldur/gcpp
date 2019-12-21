//
// Created by gelldur on 21.12.2019.
//
#include "number.hpp"

namespace gcpp::nice::number
{

std::string formatWithThousand(int value, char spaceChar /*= ' '*/)
{
	return formatWithThousand(std::to_string(value), spaceChar);
}

std::string formatWithThousand(std::string value, char spaceChar /*= ' '*/)
{
	std::string spaceCharacter(1, spaceChar);

	for(int i = (int)(value.size()) - 3; i > 0; i -= 3)
	{
		value.insert(i, spaceCharacter);
	}

	return value;
}

} // namespace gcpp::nice::number
