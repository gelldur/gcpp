// SPDX-License-Identifier: MIT
// Copyright © 2019 Dawid Drozd aka Gelldur
#pragma once

#include "string/ConstexprString.hpp"

#define S1(x) #x
#define S2(x) S1(x)

// WHERE - const char* const should be used as temporary value
// E.g. std::cout << WHERE << std::endl;
#define WHERE                                                                                      \
	(string::make(" ") + string::make(__PRETTY_FUNCTION__) + string::make(":") +                   \
	 string::make(S2(__LINE__)) + string::make(" "))                                               \
		.c_str()

// It is safe to store e.g. `constexpr auto where = WHERE_STR;`
#define WHERE_STR (string::make(__PRETTY_FUNCTION__) + ":" + string::make(S2(__LINE__)))

#define WHERE_EXCEPTION_TAG                                                                        \
	("[" + string::make(__PRETTY_FUNCTION__) + ":" + string::make(S2(__LINE__)) + "] ").toString()

template <std::size_t N>
constexpr auto trimPath(string::ConstexprString<N> path)
{
#ifdef WHERE_PROJECT_ROOT
	// For now we assume that project path & path has common prefix with length of project root
	constexpr auto projectRoot = string::make(WHERE_PROJECT_ROOT);
	static_assert(projectRoot.length() < path.length() && projectRoot.length() > 0);

	constexpr auto min =
		std::min(projectRoot.length() + (projectRoot[projectRoot.length() - 1] == '/' ? 0 : 1),
				 path.length());
	return string::subString<min>(path);
#else
#	warning "Please define WHERE_PROJECT_ROOT"
	return path; // No modification
#endif
}

// TODO make this constexpr
template <std::size_t N>
std::string erasePath(string::ConstexprString<N> path)
{
	std::string_view view(path.c_str(), path.length());
	if(auto last = view.find_last_of('/'); last != std::string_view::npos)
	{
		return std::string{view.substr(last + 1)};
	}

	return std::string{view};
}

/**
 * Macro to get e.g. my/inner/path/MyFileName.cpp:155 ([file name with project path]:[line number]
 * It depends on WHERE_PROJECT_ROOT macro which will be used to trim absolute path of file
 */
#define WHERE_PATH (trimPath(string::make(__FILE__) + ":" + string::make(S2(__LINE__))))

/**
 * Macro to get e.g. MyFileName.cpp:155 ([file name]:[line number]
 */
#define WHERE_IN_FILE (erasePath(string::make(__FILE__) + ":" + string::make(S2(__LINE__))))
