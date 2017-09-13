//
// Created by Dawid Drozd aka Gelldur on 7/11/16.
//

#pragma once

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <vector>

namespace StringUtils
{

// trim from start
inline std::string& ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
inline std::string& rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
inline std::string& trim(std::string& string)
{
	return ltrim(rtrim(string));
}

inline std::string& trim(const std::string& string)
{
	auto copy = string;
	return ltrim(rtrim(copy));
}

std::string join(const std::vector<std::string>& elements, std::string join);

inline bool endsWith(const std::string& value, const std::string& ending)
{
	if (ending.size() > value.size())
	{
		return false;
	}
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

float fuzzyCompare(const std::string& pattern, const std::string& sample);

std::vector<std::string>& split(const std::string& string, char delim, std::vector<std::string>& elements);

std::vector<std::string> split(const std::string& string, char delim);

void replaceAll(std::string& inString, const std::string& fromWhat, const std::string& toWhat);

}
