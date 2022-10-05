//
// Created by gelldur on 21.12.2019.
//
#pragma once

#include <chrono>
#include <ostream>
#include <string>

// Just use and ready to play
#define USING_NICE_TIME                                                                            \
	using namespace std::chrono_literals;                                                          \
	using gcpp::nice::time::operator<<;

namespace gcpp::nice::time
{

struct human
{
	std::chrono::nanoseconds data;
	bool show_ns : 1;
	bool show_us : 1;
	bool show_ms : 1;
	bool show_s : 1;
	bool show_min : 1;
	bool show_h : 1;
	bool show_days : 1;

	template <typename Rep, typename Period>
	constexpr explicit human(std::chrono::duration<Rep, Period>&& showFrom)
		: data(std::forward<std::chrono::duration<Rep, Period>>(showFrom))
		, show_ns{std::ratio_less_equal_v<Period, std::chrono::nanoseconds::period>}
		, show_us{std::ratio_less_equal_v<Period, std::chrono::microseconds::period>}
		, show_ms{std::ratio_less_equal_v<Period, std::chrono::milliseconds::period>}
		, show_s{std::ratio_less_equal_v<Period, std::chrono::seconds::period>}
		, show_min{std::ratio_less_equal_v<Period, std::chrono::minutes::period>}
		, show_h{std::ratio_less_equal_v<Period, std::chrono::hours::period>}
		, show_days{true}
	{ }

	template <typename Rep, typename Period>
	constexpr explicit human(const std::chrono::duration<Rep, Period>& showFrom)
		: data(showFrom)
		, show_ns{std::ratio_less_equal_v<Period, std::chrono::nanoseconds::period>}
		, show_us{std::ratio_less_equal_v<Period, std::chrono::microseconds::period>}
		, show_ms{std::ratio_less_equal_v<Period, std::chrono::milliseconds::period>}
		, show_s{std::ratio_less_equal_v<Period, std::chrono::seconds::period>}
		, show_min{std::ratio_less_equal_v<Period, std::chrono::minutes::period>}
		, show_h{std::ratio_less_equal_v<Period, std::chrono::hours::period>}
		, show_days{true}
	{ }

	friend std::ostream& operator<<(std::ostream& stream, const human& time);
};

std::ostream& operator<<(std::ostream& stream, const std::chrono::hours& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::minutes& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::seconds& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::milliseconds& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::microseconds& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::nanoseconds& duration);

/**
 * Format time in ISO 8601 (YYYY-MM-DD hh:mm:ss)
 */
std::ostream& operator<<(std::ostream& stream, const std::chrono::system_clock::time_point& time);

struct asISO8601
{
	// example: "2020-01-04 10:43:44"
	// example: "2020-01-04 10:43:44.099"
	asISO8601(const std::chrono::system_clock::time_point& time, bool withMilliseconds = false)
		: time{time}
		, withMilliseconds{withMilliseconds}
	{ }
	const std::chrono::system_clock::time_point& time;
	const bool withMilliseconds : 1;

	friend std::ostream& operator<<(std::ostream& os, const asISO8601& iso8601);
};

} // namespace gcpp::nice::time
