//
// Created by gelldur on 22.12.2019.
//
#pragma once

#include <sstream>
#include <string>

namespace gcpp::nice
{

template <typename T>
std::string toString(const T& value)
{
	std::stringstream stream;
	stream << value;
	return stream.str();
}

} // namespace gcpp::nice
