//
// Created by Dawid Drozd aka Gelldur on 7/11/16.
//

#include "StringUtils.h"

#include <sstream>

namespace StringUtils
{

std::string join(const std::vector<std::string>& elements, std::string join)
{
	std::string joined;
	for (const auto& element : elements)
	{
		if (joined.empty() == false)
		{
			joined += join;
		}

		joined += element;
	}
	return joined;
}

float fuzzyCompare(const std::string& pattern, const std::string& sample)
{
	if (pattern == sample)
	{
		return 1.F;
	}

	auto sampleSplit = split(sample, ' ');
	int allLetters = 0;
	int matching = 0;
	for (const auto& element : sampleSplit)
	{
		allLetters += element.length();
		if (pattern.find(element) != std::string::npos)
		{
			matching += element.length();
		}
	}

	if (allLetters == 0)
	{
		return 0.F;
	}

	return static_cast<float>(matching) / static_cast<float>(allLetters);
}

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss(s);
	std::string item;
	while (getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string& s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

}
