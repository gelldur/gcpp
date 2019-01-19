#pragma once

/**
 * Example of how to add Poco to your project:
 * 
 *	find_package(Poco REQUIRED Net NetSSL JSON)
 *	add_executable(weather ${SOURCES})
 *	target_include_directories(weather PRIVATE src/)
 *	target_link_libraries(weather PUBLIC Poco::Net Poco::NetSSL Poco::JSON)
 * 
 * */
#include <istream>
#include <memory>
#include <optional>
#include <string_view>

namespace Poco::Net
{
class HTTPRequest;
class HTTPResponse;
class HTTPSClientSession;
} // namespace Poco::Net

class PocoRequest
{
public:
	PocoRequest(std::string_view uri);
	~PocoRequest();

	std::string call();
	std::string call(std::optional<std::reference_wrapper<std::istream>> body);

	Poco::Net::HTTPRequest* getRequest()
	{
		return _request.get();
	}

	/**
	 * @return can return nullptr when response isn't ready
	 * */
	Poco::Net::HTTPResponse* getResponse()
	{
		return _response.get();
	}

private:
	std::unique_ptr<Poco::Net::HTTPRequest> _request;
	std::unique_ptr<Poco::Net::HTTPResponse> _response;
	std::unique_ptr<Poco::Net::HTTPSClientSession> _session;
};
