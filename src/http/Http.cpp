#include <string>
#include <iostream>
#include "Http.hpp"

/*======GETTERS======*/

httpType Http::getType() const {
	return (_type);
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

void Http::setResponse(int code) //give statuscode, set header, set body
{
	this->_statusCode = code;
}

void Http::setHeader(const std::string &key, const std::string &value)
{
	this->_headers[key] = value;
}

void Http::setBody(const std::string &body) 
{
	this->_body = body;
}


/*======UTILS======*/


Http::Http()
{
	
}

void Http::debugPrint()
{
	std::cout << "=== HTTP Debug Print ===" << std::endl;
	std::cout << "Type: " << this->getType() << std::endl;
	std::cout << "Method: " << this->getMethod() << std::endl;
	std::cout << "Version: " << this->getVersion() << std::endl;
	std::cout << "Status Code: " << this->getStatusCode() << std::endl;
	std::cout << "URI: " << this->getUri() << std::endl;
	std::cout << "Content Length: " << this->getContentLen() << std::endl;
	std::cout << "Body: " << this->getBody() << std::endl;
	std::cout << "========================" << std::endl;
}

