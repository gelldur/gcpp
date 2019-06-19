//
// Created by Dawid Drozd aka Gelldur on 06.11.17.
//
#include "beautify.h"
//////////////////////////////////

using namespace std::chrono;

namespace Beautify
{
    std::ostream& operator<<(std::ostream& stream, const Beautify::nice& time)
    {
        auto variable = time.milis;
        if (variable > 24h)
        {
            auto only = duration_cast<hours>(variable) / 24h;
            stream << only << "d ";
            variable -= (only * 24h);
        }
        if (variable > 1h)
        {
            auto only = duration_cast<hours>(variable);
            stream << only << " ";
            variable -= only;
        }
        if (variable > 1min)
        {
            auto only = duration_cast<minutes>(variable);
            stream << only << " ";
            variable -= only;
        }
        if (variable > 1s)
        {
            auto only = duration_cast<seconds>(variable);
            stream << only << " ";
            variable -= only;
        }
        if (variable > 1ms)
        {
            auto only = duration_cast<milliseconds>(variable);
            stream << only << " ";
            variable -= only;
        }

        return stream;
    }

} // namespace Beautify

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
        return stream << duration.count() << "ms";
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
        std::string date = std::ctime(&timeInfo);
        if (date.empty() == false)
        {
            date.pop_back(); // Remove extra new line
        }
        return stream << date;
    }

} // namespace std
