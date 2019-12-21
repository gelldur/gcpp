//
// Created by gelldur on 21.12.2019.
//
#include "time.hpp"

namespace gcpp::time
{

std::chrono::seconds getUnixTimeStamp()
{
	return std::chrono::duration_cast<std::chrono::seconds>(getUnixTimeStampInMilliseconds());
}

std::chrono::milliseconds getUnixTimeStampInMilliseconds()
{
	const auto now = std::chrono::system_clock::now();
	const auto duration = now.time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
}

} // namespace gcpp::time
