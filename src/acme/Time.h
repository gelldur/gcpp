//
// Created by Dawid Drozd aka Gelldur on 31.10.17.
//
#pragma once

#include <chrono>

namespace Acme
{

namespace Time
{

/**
 * @brief Return the current time in UNIX
 * @return unix time stamp in seconds
 */
std::chrono::seconds getUnixTimeStamp();

}

}


