//
// Created by gelldur on 04.01.2020.
//
#include "to_string.hpp"

#include <array>
#include <functional>
#include <string_view>

namespace gcpp::string::to_string
{

static constexpr auto LENGTH_OF_DATE = std::size("2020-01-01 00:00:00") + 1;

static void ISO8601(std::chrono::system_clock::time_point timePointUTC,
					const std::function<void(std::string_view)>& callback)
{
	const auto timeInfo = std::chrono::system_clock::to_time_t(timePointUTC);

	std::tm result{};
	/// https://linux.die.net/man/3/gmtime_r as std::gmtime is not thread safe
	gmtime_r(&timeInfo, &result); // gmtime_r is not portable, if needed please fix.

	std::array<char, LENGTH_OF_DATE> buffer{}; // Example date: "2020-01-04 10:43:44"
	const auto length = std::strftime(buffer.data(), buffer.size(), "%F %T", &result);

	callback(std::string_view{buffer.data(), length});
}

std::string formatTimeISO8601(std::chrono::system_clock::time_point timePointUTC)
{
	std::string returned{'\0', LENGTH_OF_DATE};
	ISO8601(timePointUTC,
			[&returned](std::string_view data) { returned.assign(data.data(), data.length()); });
	return returned;
}

std::ostream& asISO8601(std::ostream& stream, std::chrono::system_clock::time_point timePointUTC)
{
	ISO8601(timePointUTC, [&stream](std::string_view data) { stream << data; });
	return stream;
}

} // namespace gcpp::string::to_string
