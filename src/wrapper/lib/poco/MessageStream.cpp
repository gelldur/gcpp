//
// Created by gelldur on 16.02.19.
//

#include "MessageStream.hpp"

namespace Poco
{

MessageStream::MessageStream(std::string logger)

	: _loggerName(std::move(logger))
{}

MessageStream::MessageStream(std::string logger,
							 const std::string& source,
							 Poco::Message::Priority prio,
							 const char* file,
							 int line)
	: _message{source, "", prio, file, line}
	, _loggerName(std::move(logger))
{}

MessageStream::~MessageStream()
{
	send();
}

MessageStream& MessageStream::source(const std::string& sourceName)
{
	_message.setSource(sourceName);
	return *this;
}
MessageStream& MessageStream::debug()
{
	_message.setPriority(Poco::Message::PRIO_DEBUG);
	return *this;
}
MessageStream& MessageStream::information()
{
	_message.setPriority(Poco::Message::PRIO_INFORMATION);
	return *this;
}
MessageStream& MessageStream::warning()
{
	_message.setPriority(Poco::Message::PRIO_WARNING);
	return *this;
}
MessageStream& MessageStream::error()
{
	_message.setPriority(Poco::Message::PRIO_ERROR);
	return *this;
}
MessageStream& MessageStream::where(std::string_view fileName, int line)
{
	_message.setSourceFile(fileName.data());
	if(line != -1)
	{
		_message.setSourceLine(line);
	}
	return *this;
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
