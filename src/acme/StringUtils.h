//
// Created by Dawid Drozd aka Gelldur on 7/11/16.
//

#pragma once

#include <algorithm>
#include <string>
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

inline std::string trim(const std::string& string)
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

/**
 * @param textLeft
 * @param textRight
 * @return number of edits we need to perform to achieve sam string eg. kitten -> sitting we need 3 changes
 * 1. kitten → sitten (substitution of "s" for "k")
 * 2. sitten → sittin (substitution of "i" for "e")
 * 3. sittin → sitting (insertion of "g" at the end).
 */
int levenshteinDistance(const std::string& textLeft, const std::string& textRight);

template<typename T = std::string> // https://stackoverflow.com/a/9676623/1052261
std::vector<T> split(const T& str, const T& delimiters)
{
	std::vector<T> v;
	typename T::size_type start = 0;
	auto pos = str.find_first_of(delimiters, start);
	while (pos != T::npos)
	{
		if (pos != start)
		{ // ignore empty tokens
			v.emplace_back(str, start, pos - start);
		}
		start = pos + 1;
		pos = str.find_first_of(delimiters, start);
	}
	if (start < str.length())
	{ // ignore trailing delimiter
		v.emplace_back(str, start, str.length() - start);
	} // add what's left of the string
	return v;
}

void replaceAll(std::string& inString, const std::string& fromWhat, const std::string& toWhat);

std::string toLower(const std::string& text);

}
