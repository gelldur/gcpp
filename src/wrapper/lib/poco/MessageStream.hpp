//
// Created by gelldur on 16.02.19.
//

#pragma once

#include <ostream>
#include <sstream>

#include <Poco/Logger.h>

namespace Poco
{

class MessageStream : public std::stringstream
{
public:
	MessageStream(std::string logger);
	MessageStream(std::string logger,
				  const std::string& source,
				  Poco::Message::Priority prio,
				  const char* file,
				  int line);

	~MessageStream() override;

	MessageStream& source(const std::string& sourceName);
	MessageStream& debug();
	MessageStream& information();
	MessageStream& warning();
	MessageStream& error();
	MessageStream& where(std::string_view fileName, int line = -1);

	/**
	 * Sends message to logger. It is done automatically when dtor is called.
	 */
	void send();

private:
	Poco::Message _message;
	std::string _loggerName;
};

} // namespace Poco
