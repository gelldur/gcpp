//
// Created by Dawid Drozd aka Gelldur on 06.11.17.
//
#pragma once

#include <chrono>
#include <ostream>

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
