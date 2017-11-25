//
// Created by Dawid Drozd aka Gelldur on 06.11.17.
//
#include "beautify.h"
//////////////////////////////////

using namespace std::chrono;

namespace std
{

ostream& operator<<(ostream& stream, const hours& duration)
{
	return stream << duration.count() << "h";
}

ostream& operator<<(ostream& stream, const minutes& duration)
{
	return stream << duration.count() << "min";
}

ostream& operator<<(ostream& stream, const seconds& duration)
{
	return stream << duration.count() << "s";
}

ostream& operator<<(ostream& stream, const milliseconds& duration)
{
	return stream << duration.count() << "mls";
}

ostream& operator<<(ostream& stream, const microseconds& duration)
{
	return stream << duration.count() << "μs";
}

ostream& operator<<(ostream& stream, const nanoseconds& duration)
{
	return stream << duration.count() << "ns";
}

ostream& operator<<(ostream& stream, const system_clock::time_point& time)
{
	auto timeInfo = std::chrono::system_clock::to_time_t(time);
	return stream << std::ctime(&timeInfo);
}

}
