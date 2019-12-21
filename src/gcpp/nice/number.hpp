//
// Created by gelldur on 21.12.2019.
//
#pragma once

#include <string>

namespace gcpp::nice::number
{

/**
 * @see Utils::Formatter::formatWithThousand(int,char)
 * @param value
 * @param spaceChar
 * @return
 */
std::string formatWithThousand(int value, char spaceChar = ' ');

/**
 * Simply format such value 1234567 to 1 234 567
 * @param value value which we want format
 * @param spaceChar default space
 * @return formated string
 */
std::string formatWithThousand(std::string value, char spaceChar = ' ');

} // namespace gcpp::nice::number
