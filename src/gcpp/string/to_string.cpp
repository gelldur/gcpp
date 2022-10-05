//
// Created by gelldur on 04.01.2020.
//
#include "to_string.hpp"

#include <array>
#include <cassert>
#include <functional>
#include <string_view>

namespace gcpp::string::to_string
{

using namespace std::chrono;

// size return +1 char as extra \0, len = 19 + 1
static constexpr auto LENGTH_OF_DATE = std::size("2020-01-01 00:00:00");
// len = 23 + 1
static constexpr auto LENGTH_OF_DATE_WITH_FRAC = std::size("2020-01-01 00:00:00.000");

static void ISO8601(std::chrono::system_clock::time_point timePointUTC,
					bool withFractional,
					const std::function<void(std::string_view)>& callback)
{
	const auto timeInfo = system_clock::to_time_t(timePointUTC);

	std::tm result{};
	/// https://linux.die.net/man/3/gmtime_r as std::gmtime is not thread safe
	gmtime_r(&timeInfo, &result); // gmtime_r is not portable, if needed please fix.

	std::array<char, LENGTH_OF_DATE_WITH_FRAC> buffer{}; // Example date: "2020-01-04 10:43:44.000"
	auto length = std::strftime(buffer.data(), buffer.size(), "%F %T", &result);
	if(withFractional)
	{
		auto ms = duration_cast<milliseconds>(timePointUTC.time_since_epoch());
		unsigned fractionalSeconds = ms.count() % 1000;
		length += std::sprintf(buffer.data() + length, ".%.3u", fractionalSeconds);
	}
	assert(buffer.at(length) == '\0');
	callback(std::string_view{buffer.data(), length});
}

std::string formatTimeISO8601(std::chrono::system_clock::time_point timePointUTC,
							  bool withFractional)
{
	std::string returned((withFractional ? LENGTH_OF_DATE_WITH_FRAC : LENGTH_OF_DATE), '\0');
	ISO8601(timePointUTC, withFractional, [&returned](std::string_view data) {
		returned.assign(data.data(), data.length());
	});
	assert(returned.capacity() == (withFractional ? LENGTH_OF_DATE_WITH_FRAC : LENGTH_OF_DATE));
	assert(returned.size() == (withFractional ? LENGTH_OF_DATE_WITH_FRAC - 1 : LENGTH_OF_DATE - 1));
	return returned;
}

std::ostream& asISO8601(std::ostream& stream,
						std::chrono::system_clock::time_point timePointUTC,
						bool withFractional)
{
	ISO8601(timePointUTC, withFractional, [&stream](std::string_view data) { stream << data; });
	return stream;
}

} // namespace gcpp::string::to_string
