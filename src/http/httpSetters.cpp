#include "Http.hpp"

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