#include "PocoRequest.h"

#include <assert.h>

#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/URI.h>

PocoRequest::PocoRequest(std::string_view uri)
{
	const Poco::URI pocoUri(uri.data());
	if(pocoUri.getScheme() != "https")
	{
		throw std::invalid_argument{std::string{"Please pass https scheme not:"} + pocoUri.getScheme()};
	}

	_session = std::make_unique<Poco::Net::HTTPSClientSession>(pocoUri.getHost(), pocoUri.getPort());

	_request = std::make_unique<Poco::Net::HTTPRequest>(
	    Poco::Net::HTTPRequest::HTTP_GET, uri.data(), Poco::Net::HTTPRequest::HTTP_1_1);
}

PocoRequest::~PocoRequest() {}

std::string PocoRequest::call()
{
	return call(std::nullopt);
}

std::string PocoRequest::call(std::optional<std::reference_wrapper<std::istream>> body)
{
	assert(_session != nullptr);

	if(body.has_value())
	{
		body->get().seekg(0, std::ios::end);
		const auto size = body->get().tellg();
		_request->setContentLength(size);

		body->get().seekg(0, std::ios::beg);
	}

	std::ostream& sink = _session->sendRequest(*_request);
	if(body.has_value())
	{
		sink << body->get().rdbuf();
	}

	_response = std::make_unique<Poco::Net::HTTPResponse>();

	std::istream& responseBodyStream = _session->receiveResponse(*_response);
	return std::string(std::istreambuf_iterator<char>(responseBodyStream), {});
}
