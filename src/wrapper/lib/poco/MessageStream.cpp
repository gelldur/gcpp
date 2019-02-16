//
// Created by gelldur on 16.02.19.
//

#include "MessageStream.hpp"

namespace Poco
{
MessageStream::MessageStream(const std::string& logger,
							 const std::string& source,
							 Poco::Message::Priority prio,
							 const char* file,
							 int line)
	: _message{source, "", prio, file, line}
	, _loggerName(logger)
{}

MessageStream::~MessageStream()
{
	send();
}

void MessageStream::send()
{
	if(rdbuf()->in_avail() > 0) // check is it empty
	{
		_message.setText(str());
		Poco::Logger::get(_loggerName).log(_message);
		str(""); //clear
	}
}

} // namespace Poco
