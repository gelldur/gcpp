//
// Created by gelldur on 04.01.2020.
//
#pragma once

#include <chrono>
#include <optional>
#include <string_view>

namespace gcpp::string::from_string
{

/**
 * Parse time from ISO8601 format (YYYY-MM-DD hh:mm:ss) as UTC time
 *
 * @param text
 * @return
 */
std::optional<std::chrono::system_clock::time_point> parseTimeISO8601(std::string_view text);

} // namespace gcpp::string::from_string
