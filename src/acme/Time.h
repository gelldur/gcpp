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
         * @brief Return the current time in UNIX.
         * Unix time stamp is in UTC -> https://en.wikipedia.org/wiki/Coordinated_Universal_Time
         * @return unix time stamp in seconds
         */
        std::chrono::seconds getUnixTimeStamp();

        std::chrono::milliseconds getUnixTimeStampInMilliseconds();

    } // namespace Time

} // namespace Acme
