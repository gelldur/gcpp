//
// Created by gelldur on 04.01.2020.
//
#include "from_string.hpp"

namespace gcpp::string::from_string
{

std::optional<std::chrono::system_clock::time_point> parseTimeISO8601(std::string_view text)
{
	// This code probably could be updated when C++20 arrives
	tm parseTime{};
	// strptime and timegm are not portable if needed please fix
	if(strptime(text.data(), "%F %T", &parseTime) == nullptr)
	{
		return std::nullopt;
	}
	return std::chrono::system_clock::from_time_t(timegm(&parseTime));
}

} // namespace gcpp::string::from_string
