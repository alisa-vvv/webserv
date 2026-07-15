#include <string>
#include <iostream>
#include "../../inc/Http.hpp"

/*======CONSTRUCTOR======*/

Http::Http()
	: _state(PARSING)
	, _method(UNKNOWN)
	, _version(INVALID)
	, _statusCode(0)
	, _contentLen(0)
	, _hasBody(false)
	, _hasExtension(false)
	, requestConfig(nullptr)
{}

/*======GETTERS======*/

clientState Http::getState() const {
	return (_state);
}

httpMethod Http::getMethod() const {
	return (_method);
}

int Http::getContentLen() const {
	return (_contentLen);
}

std::string Http::getBody() const {
	return (_body);
}

std::string Http::getHeader(const std::string &key) const {
	auto it = this->_requestHeaders.find(key);
	if (it != this->_requestHeaders.end())
		return (it->second);
	return "";
}

std::string Http::getBuiltUri() const {
	return (_builtUri);
}

std::string Http::getReceivedUri() const {
	return (_receivedUri);
}

httpVersion Http::getVersion() const {
	return (_version);
}

int Http::getStatusCode() const {
	return (_statusCode);
}

/// @brief get the response string ready for client use
/// @return 
std::string Http::getResponseString() const {
	return _responseString;
}

bool Http::getExtension() const {
	return (_hasExtension);
}

/*======SETTERS======*/

/// @brief Set the status code to given status. Also sets client state to error
//if status code!= OK
/// @param code 
void Http::setResponseCode(int code) {
	this->_statusCode = code;
	if (_statusCode != HTTP_OK && _statusCode != HTTP_CREATED)
		this->_state = CLIENT_ERROR;
}

void Http::setRequestHeader(const std::string &key, const std::string &value) {
	this->_requestHeaders.insert({key, value});
}

void Http::setResponseHeader(const std::string &key, const std::string &value) {
	this->_responseHeaders.insert({key, value}); //ticket12
}

void Http::setBody() {
	//ticket20
	std::string file = this->_builtUri;
	std::ifstream fileStream(file, std::ios::binary);
	if (!fileStream.is_open())
		return setResponseCode(HTTP_FORBIDDEN);
	std::string body((std::istreambuf_iterator<char>(fileStream)),
			std::istreambuf_iterator<char>());
	this->_body = body;
}

void Http::setBody(const std::string uri) {
	if (uri.empty())
		this->_body = "";
	else
	{
		std::ifstream fileStream(uri, std::ios::binary);
		if (!fileStream.is_open())
		{
			this->_body = "";
			return;
		}
		std::string body((std::istreambuf_iterator<char>(fileStream)),
				std::istreambuf_iterator<char>());
		this->_body = body;
	}
}

void Http::setState(clientState state) {
	this->_state = state;
}

void Http::setExtension(bool status) {
	this->_hasExtension = status;
}

/*======UTILS======*/


void Http::debugPrintRequest()
{
	std::cout << "=== HTTP Debug Print ===" << std::endl;
	std::cout << "Type: " << this->getState() << std::endl;
	std::cout << "Method: " << this->getMethod() << std::endl;
	std::cout << "Version: " << this->getVersion() << std::endl;
	std::cout << "Status Code: " << this->getStatusCode() << std::endl;
	std::cout << "Content Length: " << this->getContentLen() << std::endl;
	std::cout << "Body: " << this->getBody() << std::endl;
	std::cout << "========================" << std::endl;
	
}

void Http::debugPrintRequestConfig()
{
	std::cout << "=== Request Config ====" << std::endl;
	int count = this->requestConfig->server->server_names.size();
	for (int i = 0; i < count; i++)
		std::cout << "Server names: " << this->requestConfig->server->server_names[i] << std::endl;
	std::cout << "Root: " << this->requestConfig->server->root << std::endl;
	std::cout << "Location: prefix" << this->requestConfig->location->prefix << std::endl;
	
}
