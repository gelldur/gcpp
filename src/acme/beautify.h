//
// Created by Dawid Drozd aka Gelldur on 06.11.17.
//
#pragma once

#include <chrono>
#include <ostream>

namespace Beautify
{
struct nice
{
	std::chrono::nanoseconds data;
	bool show_ns : 1;
	bool show_us : 1;
	bool show_ms : 1;
	bool show_s : 1;
	bool show_min : 1;
	bool show_h : 1;
	bool show_days : 1;

	explicit nice(const std::chrono::nanoseconds& nanos)
		: data(nanos)
		, show_ns{true}
		, show_us{true}
		, show_ms{true}
		, show_s{true}
		, show_min{true}
		, show_h{true}
		, show_days{true}
	{}

	explicit nice(const std::chrono::microseconds& nanos)
		: data(nanos)
		, show_ns{false}
		, show_us{true}
		, show_ms{true}
		, show_s{true}
		, show_min{true}
		, show_h{true}
		, show_days{true}
	{}

	explicit nice(const std::chrono::milliseconds& nanos)
		: data(nanos)
		, show_ns{false}
		, show_us{false}
		, show_ms{true}
		, show_s{true}
		, show_min{true}
		, show_h{true}
		, show_days{true}
	{}

	explicit nice(const std::chrono::seconds& nanos)
		: data(nanos)
		, show_ns{false}
		, show_us{false}
		, show_ms{false}
		, show_s{true}
		, show_min{true}
		, show_h{true}
		, show_days{true}
	{}

	friend std::ostream& operator<<(std::ostream& stream, const nice& time);
};
} // namespace Beautify

namespace std
{

std::ostream& operator<<(std::ostream& stream, const std::chrono::hours& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::minutes& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::seconds& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::milliseconds& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::microseconds& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::nanoseconds& duration);

std::ostream& operator<<(std::ostream& stream, const std::chrono::system_clock::time_point& time);

} // namespace std
