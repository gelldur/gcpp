//
// Created by gelldur on 05/10/22.
//
#pragma once

#include <ios>

namespace gcpp::stream
{

/**
 * RAII approach to restore format for stream.
 * Example:
 * 		stream << std::setw(3) << std::setfill('0') << 5;
 * Now `setfill` is permanent and we would need to restore previous fill char.
 *
 * Thanks to this guard it will be done automatically.
 *
 * Example:
 * 		{
 * 			StreamScopeFormat guard{stream};
 * 			stream << std::setw(3) << std::setfill('0') << 5; // will display '005'
 * 		}
 * 		stream << std::setw(3) << 5; // will display '  5'
 *
 *
 * @tparam T
 */
template <typename T>
struct StreamScopeFormat
{
	explicit StreamScopeFormat(std::basic_ios<T>& stream)
		: init{nullptr}
		, stream{stream}
	{
		init.copyfmt(stream);
	}

	~StreamScopeFormat()
	{
		stream.copyfmt(init); // restore format
	}

	std::ios init;
	std::basic_ios<T>& stream;
};

} // namespace gcpp::stream
