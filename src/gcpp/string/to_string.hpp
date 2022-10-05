//
// Created by gelldur on 04.01.2020.
//
#pragma once

#include <chrono>
#include <string>

namespace gcpp::string::to_string
{

/**
 * Format time in ISO 8601 (YYYY-MM-DD hh:mm:ss) as UTC
 *
 * @param timePointUTC
 * @return
 */
std::string formatTimeISO8601(std::chrono::system_clock::time_point timePointUTC,
							  bool withFractional = false);

std::ostream& asISO8601(std::ostream& stream,
						std::chrono::system_clock::time_point timePointUTC,
						bool withFractional = false);

} // namespace gcpp::string::to_string
