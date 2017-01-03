//
// Created by Dawid Drozd aka Gelldur on 6/30/16.
//

#include "Fail.h"

#include <stdexcept>

#include <log.h>

Fail::Fail(const char* fileName, const char* functionName, int lineNumber)
		: _lineNumber(lineNumber)
		, _fileName(fileName)
		, _functionName(functionName)
{
	auto srcPosition = _fileName.rfind("/src/");
	if (srcPosition != std::string::npos)
	{
		_fileName.erase(0, srcPosition);
	}
	_stream << _fileName << " " << _fileName << ":" << _lineNumber << " ";
}

void Fail::report() const
{
	auto message = _stream.str();
	ELOG("Fail: %s", message.c_str());
	if (_crush)
	{
		throw std::runtime_error(message);
	}
}

void Fail::If(bool valueToCheck, const Fail& fail)
{
	if (valueToCheck)
	{
		fail.report();
	}
}

Fail& Fail::crush(bool shouldCrush)
{
	_crush = shouldCrush;
	return *this;
}
