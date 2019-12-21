//
// Created by gelldur on 21.12.2019.
//
#include "gcpp/nice/time.hpp"

#include <iomanip>

using namespace std::chrono;

namespace gcpp::nice::time
{

std::ostream& operator<<(std::ostream& out, const human& time)
{
	std::stringstream stream;
	auto variable = time.data;
	if(time.show_days && variable > 24h)
	{
		auto only = duration_cast<hours>(variable) / 24h;
		stream << std::setw(3) << only << "d ";
		variable -= (only * 24h);
	}
	if(time.show_h && variable > 1h)
	{
		auto only = duration_cast<hours>(variable);
		stream << " " << std::setw(3) << only;
		variable -= only;
	}
	if(time.show_min && variable > 1min)
	{
		auto only = duration_cast<minutes>(variable);
		stream << " " << std::setw(3) << only;
		variable -= only;
	}
	if(time.show_s && variable > 1s)
	{
		auto only = duration_cast<seconds>(variable);
		stream << " " << std::setw(3) << only;
		variable -= only;
	}
	if(time.show_ms && variable > 1ms)
	{
		auto only = duration_cast<milliseconds>(variable);
		stream << " " << std::setw(3) << only;
		variable -= only;
	}
	if(time.show_us && variable > 1us)
	{
		auto only = duration_cast<microseconds>(variable);
		stream << " " << std::setw(3) << only;
		variable -= only;
	}
	if(time.show_ns && variable > 1ns)
	{
		auto only = duration_cast<nanoseconds>(variable);
		stream << " " << std::setw(3) << only;
		variable -= only;
	}

	out << stream.str();
	return out;
}

std::ostream& operator<<(std::ostream& stream, const hours& duration)
{
	return stream << duration.count() << "h";
}

std::ostream& operator<<(std::ostream& stream, const minutes& duration)
{
	return stream << duration.count() << "min";
}

std::ostream& operator<<(std::ostream& stream, const seconds& duration)
{
	return stream << duration.count() << "s";
}

std::ostream& operator<<(std::ostream& stream, const milliseconds& duration)
{
	return stream << duration.count() << "ms";
}

std::ostream& operator<<(std::ostream& stream, const microseconds& duration)
{
	return stream << duration.count() << "μs";
}

std::ostream& operator<<(std::ostream& stream, const nanoseconds& duration)
{
	return stream << duration.count() << "ns";
}

std::ostream& operator<<(std::ostream& stream, const system_clock::time_point& time)
{
	auto timeInfo = system_clock::to_time_t(time);
	std::string date = std::ctime(&timeInfo);
	if(date.empty() == false)
	{
		date.pop_back(); // Remove extra new line
	}
	return stream << date;
}

} // namespace gcpp::nice::time
