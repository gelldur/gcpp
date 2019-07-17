//
// Created by Dawid Drozd aka Gelldur on 6/30/16.
//
#pragma once

#include <string_view>

/**
 * Consider using:
 * - common-cpp/src/string/ConstexprString.hpp
 * - common-cpp/src/acme/where.hpp
 *
 * Example call:
 * commoncpp::exception::fail_if_false(myName == "Gelldur", WHERE);
 *
 * Other example why I believe why this is more readable:
 *
 * commoncpp::exception::fail_if_true(std::any_of(name.begin(), name.end(), ::isspace), WHERE);
 * vs
 * ASSERT(std::any_of(name.begin(), name.end(), ::isspace) == false);
 *
 * assert is always confusing... it fails on true or on false ?
 * Thanks to if_false/if_true we don't have to use:
 * - value == false
 * - value != false
 * - !value (negate)
 * - not value (negate)
 */

namespace commoncpp::exception
{

void fail_here(std::string_view where) noexcept(false);

void fail_if_false(bool valueToCheck, std::string_view where) noexcept(false);
void fail_if_true(bool valueToCheck, std::string_view where) noexcept(false);

} // namespace commoncpp::exception
