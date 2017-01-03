#pragma once

#include <string>

namespace Utils
{

namespace Formatter
{

std::string formatWithThousand(int value, char spaceChar = ' ');
std::string formatWithThousand(std::string value, char spaceChar = ' ');

std::string formatTime(long long time);
std::string formatTimeWithoutMilliseconds(long long time);

}

} /* namespace Utils */
