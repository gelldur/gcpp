#pragma once

#include <cstddef>
#include <source_location>
#include <string_view>

#include "gcpp/string/ConstexprString.hpp"

/**
 * @file
 * Compile-time "where am I" building blocks: file path, file name, line, function name.
 *
 * Every function in @ref gcpp::where takes `std::source_location` defaulted to
 * `std::source_location::current()`. A default argument is evaluated at the *call site*, so a
 * plain call `where::line()` reports the caller's line - no macro needed. Compose your own tags
 * the same way, and always forward `location` to the inner calls:
 *
 * @code
 * consteval auto logTag(const std::source_location location = std::source_location::current())
 * {
 * 	return "<" + gcpp::where::fileName(location) + "@" + gcpp::where::line(location) + ">";
 * }
 * // runtime use - only the exact final text lands in the binary:
 * std::puts(gcpp::string::STATIC_C_STR<logTag()>);
 * @endcode
 *
 * Project root trimming: the recommended way is the compiler flag
 * `-fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=` (GCC >= 8, Clang >= 10). It rewrites `__FILE__` and
 * `std::source_location::file_name()` itself, so nothing here has to do it and absolute build
 * paths don't leak into the binary. @ref gcpp::trimPath exists for when you cannot change flags.
 */

#ifndef GCPP_WHERE_FILE_PATH_CAPACITY
/// Max length of a file path from `std::source_location`. Overflow is a compile error.
#	define GCPP_WHERE_FILE_PATH_CAPACITY 256
#endif

#ifndef GCPP_WHERE_FUNCTION_NAME_CAPACITY
/// Max length of a pretty function name (templates get long). Overflow is a compile error.
#	define GCPP_WHERE_FUNCTION_NAME_CAPACITY 1024
#endif

namespace gcpp
{

/**
 * Removes directories, keeps file name with extension. Accepts '/' and '\\' (MSVC `__FILE__`).
 * @code
 * erasePath(ConstexprString{"/src/core/Order.cpp"}); // "Order.cpp"
 * @endcode
 */
template <std::size_t N>
consteval gcpp::string::ConstexprString<N> erasePath(const gcpp::string::ConstexprString<N>& path)
{
	if(const auto last = gcpp::string::detail::contentOf(path).find_last_of("/\\");
	   last != std::string_view::npos)
	{
		return gcpp::string::subString(path, last + 1);
	}
	return path;
}

/**
 * Removes the last extension, only inside the last path component - same rules as
 * std::filesystem::path::stem(): `"a.tar.gz"` -> `"a.tar"`, `".hidden"` stays `".hidden"`,
 * `"dir.v2/file"` stays untouched.
 */
template <std::size_t N>
consteval gcpp::string::ConstexprString<N> eraseExtension(
	const gcpp::string::ConstexprString<N>& path)
{
	const std::string_view view = gcpp::string::detail::contentOf(path);
	const auto separator = view.find_last_of("/\\");
	const std::size_t nameStart = separator == std::string_view::npos ? 0 : separator + 1;
	const auto dot = view.rfind('.');

	if(dot == std::string_view::npos || dot <= nameStart)
	{
		return path; // no extension, or a dot-file like ".hidden"
	}
	return gcpp::string::subString(path, 0, dot);
}

/**
 * Strips `projectRoot` prefix (and the separator following it) from `path`.
 * Path outside the root (e.g. system/3rd party headers) is returned unchanged, it is not an error.
 * The prefix must match whole directory names: root "/home/a" does not trim "/home/ab/x".
 * @code
 * trimPath(ConstexprString{"/home/me/rr/src/a.cpp"}, ConstexprString{"/home/me/rr"}); //
 * "src/a.cpp"
 * @endcode
 */
template <std::size_t N, std::size_t R>
consteval gcpp::string::ConstexprString<N> trimPath(
	const gcpp::string::ConstexprString<N>& path,
	const gcpp::string::ConstexprString<R>& projectRoot)
{
	const std::string_view view = gcpp::string::detail::contentOf(path);
	const std::string_view root = gcpp::string::detail::contentOf(projectRoot);
	if(root.empty() || !view.starts_with(root))
	{
		return path;
	}

	const bool rootEndsWithSeparator = root.back() == '/' || root.back() == '\\';
	if(rootEndsWithSeparator)
	{
		return gcpp::string::subString(path, root.size());
	}
	if(view.size() > root.size() && (view[root.size()] == '/' || view[root.size()] == '\\'))
	{
		return gcpp::string::subString(path, root.size() + 1);
	}
	return path; // only a partial directory name matched
}

/**
 * Strips `WHERE_PROJECT_ROOT` (define it e.g. via CMake:
 * `target_compile_definitions(app PRIVATE WHERE_PROJECT_ROOT="${CMAKE_SOURCE_DIR}")`).
 * Without the define the path is returned unchanged.
 */
template <std::size_t N>
consteval gcpp::string::ConstexprString<N> trimPath(const gcpp::string::ConstexprString<N>& path)
{
#ifdef WHERE_PROJECT_ROOT
	return trimPath(path, gcpp::string::ConstexprString{WHERE_PROJECT_ROOT});
#else
	return path;
#endif
}

} // namespace gcpp

namespace gcpp::where
{

inline constexpr std::size_t FILE_PATH_CAPACITY = GCPP_WHERE_FILE_PATH_CAPACITY;
inline constexpr std::size_t FUNCTION_NAME_CAPACITY = GCPP_WHERE_FUNCTION_NAME_CAPACITY;

/// Whole file path exactly as the compiler saw it, e.g. "/home/me/rr/src/core/Order.cpp".
consteval gcpp::string::ConstexprString<FILE_PATH_CAPACITY> filePath(
	const std::source_location location = std::source_location::current())
{
	return gcpp::string::ConstexprString<FILE_PATH_CAPACITY>{location.file_name()};
}

/// File path relative to `WHERE_PROJECT_ROOT` (unchanged when not defined), e.g.
/// "src/core/Order.cpp".
consteval gcpp::string::ConstexprString<FILE_PATH_CAPACITY> projectFilePath(
	const std::source_location location = std::source_location::current())
{
	return gcpp::trimPath(filePath(location));
}

/// e.g. "Order.cpp"
consteval gcpp::string::ConstexprString<FILE_PATH_CAPACITY> fileNameWithExtension(
	const std::source_location location = std::source_location::current())
{
	return gcpp::erasePath(filePath(location));
}

/// e.g. "Order"
consteval gcpp::string::ConstexprString<FILE_PATH_CAPACITY> fileName(
	const std::source_location location = std::source_location::current())
{
	return gcpp::eraseExtension(fileNameWithExtension(location));
}

/// e.g. "42"
consteval gcpp::string::ConstexprString<gcpp::string::INTEGER_CAPACITY> line(
	const std::source_location location = std::source_location::current())
{
	return gcpp::string::fromInteger(location.line());
}

/**
 * Same text as `__PRETTY_FUNCTION__` on GCC and Clang, e.g. "void tf(T) [with T = int]".
 * Outside of any function (namespace scope initializer) it is empty.
 */
consteval gcpp::string::ConstexprString<FUNCTION_NAME_CAPACITY> functionName(
	const std::source_location location = std::source_location::current())
{
	return gcpp::string::ConstexprString<FUNCTION_NAME_CAPACITY>{location.function_name()};
}

/**
 * Replacement for the old `WHERE_EXCEPTION_TAG` macro: "[<pretty function>:<line>] ".
 * @code
 * throw std::runtime_error(std::string{gcpp::string::STATIC_VIEW<gcpp::where::exceptionTag()>}
 *                          + "Order rejected");
 * @endcode
 */
consteval auto exceptionTag(const std::source_location location = std::source_location::current())
{
	return "[" + functionName(location) + ":" + line(location) + "] ";
}

} // namespace gcpp::where

/// @name Macro building blocks
/// `ConstexprString` values describing the macro expansion point. Prefer the gcpp::where
/// functions; these exist where a macro reads better. GCPP_FILE_PATH and GCPP_FUNCTION_NAME have
/// capacity == length, so they are also cheaper to compile than the source_location versions.
/// @{
#define GCPP_FILE_PATH ::gcpp::string::STATIC_VIEW<::gcpp::where::filePath()>
#define GCPP_FILE_NAME_WITH_EXTENSION                                                              \
	::gcpp::string::STATIC_VIEW<::gcpp::where::fileNameWithExtension()>
#define GCPP_FILE_NAME ::gcpp::string::STATIC_VIEW<::gcpp::where::fileName()>
#define GCPP_LINE ::gcpp::string::STATIC_VIEW<::gcpp::where::line()>
#define GCPP_FUNCTION_NAME ::gcpp::string::STATIC_VIEW<::gcpp::where::functionName()>
/// @}
