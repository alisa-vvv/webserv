#include <string>
#include <iostream>
#include "Http.hpp"

/*======CONSTRUCTOR======*/

Http::Http()
	: _state(PARSING)
	, _method(UNKNOWN)
	, _version(INVALID)
	, _statusCode(0)
	, _contentLen(0)
	, _hasBody(false)
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
	auto it = this->_headers.find(key);
	if (it != this->_headers.end())
		return (it->second);
	return "";
}

std::string Http::getUri() const {
	return (_uri);
}

httpVersion Http::getVersion() const {
	return (_version);
}

int Http::getStatusCode() const {
	return (_statusCode);
}

// std::string Http::getResponseString() { //back to raw string for response
// 	return (_res)
// }

/*======SETTERS======*/

/// @brief Set the status code to given status. Also sets client state to error
//if status code!= OK
/// @param code 
void Http::setResponseCode(int code)
{
	this->_statusCode = code;
	if (_statusCode != HTTP_OK && _statusCode != HTTP_CREATED)
		this->_state = ERROR;
}

// void Http::setHeader(const std::string &key, const std::string &value)
// {
// 	this->_headers[key] = value;
// }

void Http::setBody(const std::string &body) 
{
	this->_body = body;
}

void Http::setState(clientState state)
{
	this->_state = state;
}

/*======UTILS======*/


void Http::debugPrint()
{
	std::cout << "=== HTTP Debug Print ===" << std::endl;
	std::cout << "Type: " << this->getState() << std::endl;
	std::cout << "Method: " << this->getMethod() << std::endl;
	std::cout << "Version: " << this->getVersion() << std::endl;
	std::cout << "Status Code: " << this->getStatusCode() << std::endl;
	std::cout << "URI: " << this->getUri() << std::endl;
	std::cout << "Content Length: " << this->getContentLen() << std::endl;
	std::cout << "Body: " << this->getBody() << std::endl;
	std::cout << "========================" << std::endl;
}

