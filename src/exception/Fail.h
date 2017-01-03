//
// Created by Dawid Drozd aka Gelldur on 6/30/16.
//

#pragma once

#include <sstream>

class Fail
{
public:
	static void If(bool valueToCheck, const Fail& fail);
	Fail(const char* fileName, const char* functionName, int lineNumber);

	template<class T>
	Fail& add(const T& message)
	{
		_stream << message;
		return *this;
	}

	Fail& br()
	{
		_stream << '\n';
		return *this;
	}

	void report() const;
	Fail& crush(bool shouldCrush = true);

private:
	bool _crush = true;
	int _lineNumber;
	std::string _fileName;
	std::string _functionName;
	std::stringstream _stream;
};


