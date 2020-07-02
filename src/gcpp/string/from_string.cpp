//
// Created by gelldur on 04.01.2020.
//
#include "from_string.hpp"

#include <cstdio>

namespace gcpp::string::from_string
{

// Example format: 2020-04-16T12:46:33Z
static constexpr std::string_view format_ISO8601{"%d-%d-%d%*c%d:%d:%d"};
static constexpr std::string_view format_ISO8601WithFraction{"%d-%d-%d%*c%d:%d:%d.%dZ"};

/** https://www.w3.org/TR/NOTE-datetime
   Year:
	  YYYY (eg 1997)
   Year and month:
	  YYYY-MM (eg 1997-07)
   Complete date:
	  YYYY-MM-DD (eg 1997-07-16)
   Complete date plus hours and minutes:
	  YYYY-MM-DDThh:mmTZD (eg 1997-07-16T19:20+01:00)
   Complete date plus hours, minutes and seconds:
	  YYYY-MM-DDThh:mm:ssTZD (eg 1997-07-16T19:20:30+01:00)
   Complete date plus hours, minutes, seconds and a decimal fraction of a
second
	  YYYY-MM-DDThh:mm:ss.sTZD (eg 1997-07-16T19:20:30.45+01:00)
where:

	 YYYY = four-digit year
	 MM   = two-digit month (01=January, etc.)
	 DD   = two-digit day of month (01 through 31)
	 hh   = two digits of hour (00 through 23) (am/pm NOT allowed)
	 mm   = two digits of minute (00 through 59)
	 ss   = two digits of second (00 through 59)
	 s    = one or more digits representing a decimal fraction of a second
	 TZD  = time zone designator (Z or +hh:mm or -hh:mm)
 */

// TODO need to do better this
std::optional<std::chrono::system_clock::time_point> parseTimeISO8601(std::string_view text)
{
	if(text.size() < std::size(format_ISO8601))
	{
		return std::nullopt;
	}

	// This code probably could be updated when C++20 arrives
	tm parseTime{};

	int y = 0, M = 0, d = 1, h = 0, m = 0, s = 0;
	if(sscanf(text.data(), format_ISO8601.data(), &y, &M, &d, &h, &m, &s) != 6)
	{
		return std::nullopt;
	}

	parseTime.tm_year = y - 1900; // Year since 1900
	if(parseTime.tm_year < 0)
	{
		return std::nullopt;
	}
	parseTime.tm_mon = M - 1; // 0-11
	if(parseTime.tm_mon < 0 || parseTime.tm_mon > 11)
	{
		return std::nullopt;
	}

	parseTime.tm_mday = d; // 1-31
	if(parseTime.tm_mday < 1 || parseTime.tm_mday > 31)
	{
		return std::nullopt;
	}

	parseTime.tm_hour = h; // 0-23
	if(parseTime.tm_hour < 0 || parseTime.tm_hour > 23)
	{
		return std::nullopt;
	}

	parseTime.tm_min = m; // 0-59
	if(parseTime.tm_min < 0 || parseTime.tm_min > 59)
	{
		return std::nullopt;
	}

	parseTime.tm_sec = s; // 0-61 (0-60 in C++11)
	if(parseTime.tm_min < 0 || parseTime.tm_min > 61)
	{
		return std::nullopt;
	}

	parseTime.tm_isdst = 0;

	return std::chrono::system_clock::from_time_t(timegm(&parseTime));
}

std::optional<std::chrono::system_clock::time_point> parseTimeISO8601WithFraction(
	std::string_view text)
{
	if(text.size() < std::size(format_ISO8601)) // We may skip millisecond part
	{
		return std::nullopt;
	}

	// This code probably could be updated when C++20 arrives
	tm parseTime{};

	int y = 0, M = 0, d = 1, h = 0, m = 0, s = 0, ms = 0;
	// We may skip parsing milliseconds when not available
	if(sscanf(text.data(), format_ISO8601WithFraction.data(), &y, &M, &d, &h, &m, &s, &ms) < 6)
	{
		return std::nullopt;
	}
	parseTime.tm_year = y - 1900; // Year since 1900
	parseTime.tm_mon = M - 1; // 0-11
	parseTime.tm_mday = d; // 1-31
	parseTime.tm_hour = h; // 0-23
	parseTime.tm_min = m; // 0-59
	parseTime.tm_sec = s; // 0-61 (0-60 in C++11)
	parseTime.tm_isdst = 0;

	return std::chrono::system_clock::from_time_t(timegm(&parseTime)) +
		   std::chrono::milliseconds{ms};
}

} // namespace gcpp::string::from_string
