//
// Created by gelldur on 21.12.2019.
//
#include "gcpp/nice/time.hpp"

#include <sstream>
#include <iomanip>

#include "gcpp/stream/StreamScopeFormat.hpp"
#include "gcpp/string/to_string.hpp"

namespace gcpp::nice::time
{

std::ostream& operator<<(std::ostream& out, const human& time)
{
	using namespace std::chrono;

	std::stringstream stream;
	auto variable = time.data;
	const auto width = out.width();
	bool addSpace = false;

	if(time.show_days && variable >= 24h)
	{
		const auto only = duration_cast<hours>(variable) / 24h;
		stream << std::setw(width) << only << "d";
		variable -= (only * 24h);
		addSpace = true;
	}
	if(time.show_h && variable >= 1h)
	{
		if(addSpace)
		{
			stream << out.fill();
		}

		const auto only = duration_cast<hours>(variable);
		stream << std::setw(width) << only;
		variable -= only;
		addSpace = true;
	}
	if(time.show_min && variable >= 1min)
	{
		if(addSpace)
		{
			stream << out.fill();
		}

		const auto only = duration_cast<minutes>(variable);
		stream << std::setw(width) << only;
		variable -= only;
		addSpace = true;
	}
	if(time.show_s && variable >= 1s)
	{
		if(addSpace)
		{
			stream << out.fill();
		}

		const auto only = duration_cast<seconds>(variable);
		stream << std::setw(width) << only;
		variable -= only;
		addSpace = true;
	}
	if(time.show_ms && variable >= 1ms)
	{
		if(addSpace)
		{
			stream << out.fill();
		}

		const auto only = duration_cast<milliseconds>(variable);
		stream << std::setw(width) << only;
		variable -= only;
		addSpace = true;
	}
	if(time.show_us && variable >= 1us)
	{
		if(addSpace)
		{
			stream << out.fill();
		}

		const auto only = duration_cast<microseconds>(variable);
		stream << std::setw(width) << only;
		variable -= only;
		addSpace = true;
	}
	if(time.show_ns && variable >= 1ns)
	{
		if(addSpace)
		{
			stream << out.fill();
		}

		const auto only = duration_cast<nanoseconds>(variable);
		stream << std::setw(width) << only;
		variable -= only;
		addSpace = true;
	}

	out << stream.str();
	return out;
}

std::ostream& operator<<(std::ostream& os, const asISO8601& iso8601)
{
	gcpp::string::to_string::asISO8601(os, iso8601.time, iso8601.withMilliseconds);
	return os;
}

std::ostream& operator<<(std::ostream& stream, const std::chrono::system_clock::time_point& time)
{
	return gcpp::string::to_string::asISO8601(stream, time);
}

std::ostream& operator<<(std::ostream& stream, const std::chrono::hours& duration)
{
	return stream << duration.count() << "h";
}

std::ostream& operator<<(std::ostream& stream, const std::chrono::minutes& duration)
{
	return stream << duration.count() << "min";
}

std::ostream& operator<<(std::ostream& stream, const std::chrono::seconds& duration)
{
	return stream << duration.count() << "s";
}

std::ostream& operator<<(std::ostream& stream, const std::chrono::milliseconds& duration)
{
	return stream << duration.count() << "ms";
}

std::ostream& operator<<(std::ostream& stream, const std::chrono::microseconds& duration)
{
	// From ASCII
	// DEC		OCT		HEX		BIN			Symbol	HTML Number		HTML Name
	// 181		265		B5		10110101	µ		&#181;			&micro;			Micro sign
	static_assert(sizeof("µ") == 3); // extra byte for \0
	static_assert(sizeof("µs") == 4); // extra byte for \0
	return stream << duration.count() << "µs";
}

std::ostream& operator<<(std::ostream& stream, const std::chrono::nanoseconds& duration)
{
	return stream << duration.count() << "ns";
}

} // namespace gcpp::nice::time
