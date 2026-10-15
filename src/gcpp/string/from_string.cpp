//
// Created by gelldur on 04.01.2020.
//
#include "from_string.hpp"

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string_view>

namespace gcpp::string::from_string
{

#if __cplusplus >= 202002L // C++20

class string_view_streambuf : public std::streambuf
{
public:
	string_view_streambuf(std::string_view sv)
	{
		char* p = const_cast<char*>(sv.data());
		setg(p, p, p + sv.size());
	}
};

class string_view_istream : public std::istream
{
public:
	string_view_istream(std::string_view sv)
		: std::istream(&buf_)
		, buf_(sv)
	{ }

private:
	string_view_streambuf buf_;
};

std::optional<std::chrono::system_clock::time_point> parseTimeISO8601(std::string_view text)
{
	const std::array<std::string, 2> formats = {
		"%Y-%m-%dT%H:%M:%S",
		"%Y-%m-%d %H:%M:%S",
	};
	std::chrono::sys_time<std::chrono::seconds> tp;
	for(const auto& f : formats)
	{
		string_view_istream is{text};
		is >> std::chrono::parse(f, tp);

		if(is.fail() == false)
		{
			return tp;
		}
	}
	return std::nullopt;
}

std::optional<std::chrono::system_clock::time_point> parseTimeISO8601WithFraction(
	std::string_view text)
{
	const std::array<std::string, 2> formats = {
		"%Y-%m-%dT%H:%M:%S",
		"%Y-%m-%d %H:%M:%S",
	};

	std::chrono::sys_time<std::chrono::milliseconds> tp;

	for(const auto& f : formats)
	{
		string_view_istream is{text};
		is >> std::chrono::parse(f, tp);

		if(is.fail() == false)
		{
			return tp;
		}
	}
	return std::nullopt;
}

#else

// Example format: 2020-04-16T12:46:33Z
static constexpr std::string_view format_ISO8601{"%d-%d-%d%*c%d:%d:%d"};
// static constexpr std::string_view format_ISO8601WithFraction{"%d-%d-%d%*c%d:%d:%d.%dZ"};

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

	int y = -1, M = -1, d = -1, h = -1, m = -1, s = -1;
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
	if(parseTime.tm_sec < 0 || parseTime.tm_sec > 61)
	{
		return std::nullopt;
	}

	parseTime.tm_isdst = 0;

	return std::chrono::system_clock::from_time_t(timegm(&parseTime));
}

std::optional<std::chrono::system_clock::time_point> parseTimeISO8601WithFraction(
	std::string_view text)
{
	auto t = parseTimeISO8601(text);
	if(t.has_value() == false)
	{
		return std::nullopt;
	}

	const auto fractionPosition = text.find('.');
	if(fractionPosition != std::string_view::npos) // If we don't have millisecond part we may skip
	{
		int ms = -1;
		if(sscanf(text.substr(fractionPosition).data(), "%dZ", &ms) < 1)
		{
			return std::nullopt;
		}

		return *t + std::chrono::milliseconds{ms};
	}
	else
	{
		return t;
	}
}

#endif

} // namespace gcpp::string::from_string
