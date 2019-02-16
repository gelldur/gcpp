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
	MessageStream(const std::string& logger,
				  const std::string& source,
				  Poco::Message::Priority prio,
				  const char* file,
				  int line);

	virtual ~MessageStream() override;

	/**
	 * Sends message to logger. It is done automatically when dtor is called.
	 */
	void send();

private:
	Poco::Message _message;
	std::string _loggerName;
};

} // namespace Poco
