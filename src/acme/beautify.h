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
	std::chrono::milliseconds milis;

	nice(const std::chrono::nanoseconds& milis)
			: milis(std::chrono::duration_cast<std::chrono::milliseconds>(milis))
	{
	}

	friend std::ostream& operator<<(std::ostream& stream, const nice& time);
};
}

namespace std
{

std::ostream& operator<<(std::ostream& stream, const std::chrono::hours& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::minutes& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::seconds& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::milliseconds& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::microseconds& duration);
std::ostream& operator<<(std::ostream& stream, const std::chrono::nanoseconds& duration);

std::ostream& operator<<(std::ostream& stream, const std::chrono::system_clock::time_point& time);

}
