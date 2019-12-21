//
// Created by Dawid Drozd aka Gelldur on 7/11/16.
//

#include "acme.hpp"

#include <numeric>
#include <sstream>

namespace gcpp::string
{

std::string join(const std::vector<std::string>& elements, std::string join)
{
	std::string joined;
	for(const auto& element : elements)
	{
		if(joined.empty() == false)
		{
			joined += join;
		}

		joined += element;
	}
	return joined;
}

float fuzzyCompare(const std::string& pattern, const std::string& sample)
{
	if(pattern == sample)
	{
		return 1.F;
	}

	auto sampleSplit = split<std::string>(sample, " ");
	int allLetters = 0;
	int matching = 0;
	for(const auto& element : sampleSplit)
	{
		allLetters += element.length();
		if(pattern.find(element) != std::string::npos)
		{
			matching += element.length();
		}
	}

	if(allLetters == 0)
	{
		return 0.F;
	}

	return static_cast<float>(matching) / static_cast<float>(allLetters);
}

// Implementation from:
// https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C.2B.2B
int levenshteinDistance(const std::string& textLeft, const std::string& textRight)
{
	// To change the type this function manipulates and returns, change
	// the return type and the types of the two variables below.
	int s1len = textLeft.size();
	int s2len = textRight.size();

	auto column_start = (decltype(s1len))1;

	auto column = new decltype(s1len)[s1len + 1];
	std::iota(column + column_start, column + s1len + 1, column_start);

	for(auto x = column_start; x <= s2len; x++)
	{
		column[0] = x;
		auto last_diagonal = x - column_start;
		for(auto y = column_start; y <= s1len; y++)
		{
			auto old_diagonal = column[y];
			auto possibilities = {column[y] + 1,
								  column[y - 1] + 1,
								  last_diagonal + (textLeft[y - 1] == textRight[x - 1] ? 0 : 1)};
			column[y] = std::min(possibilities);
			last_diagonal = old_diagonal;
		}
	}
	auto result = column[s1len];
	delete[] column;
	return result;
}

void replaceAll(std::string& inString, const std::string& fromWhat, const std::string& toWhat)
{
	if(fromWhat.empty())
	{
		return;
	}
	size_t position = 0;
	while((position = inString.find(fromWhat, position)) != std::string::npos)
	{
		inString.replace(position, fromWhat.length(), toWhat);
		position += toWhat.length();
	}
}

std::string toLower(const std::string& text)
{
	auto lowerCaseString = text;
	std::transform(
		lowerCaseString.begin(), lowerCaseString.end(), lowerCaseString.begin(), ::tolower);
	return lowerCaseString;
}

} // namespace gcpp::string
