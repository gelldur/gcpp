//
// Created by gelldur on 21.12.2019.
//
#pragma once

#include <chrono>

namespace gcpp::time
{

/**
 * @brief Return the current time in UNIX.
 * Unix time stamp is in UTC -> https://en.wikipedia.org/wiki/Coordinated_Universal_Time
 * @return unix time stamp in seconds
 */
std::chrono::seconds getUnixTimeStamp();

std::chrono::milliseconds getUnixTimeStampInMilliseconds();

} // namespace gcpp::time
