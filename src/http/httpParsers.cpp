#include "Http.hpp"


//tRequestString = "POST /api/users?active=true HTTP/1.1\r\nHost: api.example.com\r\nContent-Type: application/json\r\nContent-Length: 37\r\nConnection: keep-alive\r\nAuthorization: Bearer eyJhbGci...\r\n\r\n{"username": "ally", "active": true}";


void Http::parseRequestLine(const std::string line)
{
	size_t firstSep = line.find(' ');
	size_t	secSep = line.rfind(' ');

	if (firstSep == std::string::npos || secSep == std::string::npos)
		throw HttpException(400);
	std::string method = line.substr(0, firstSep);
	std::string uri = line.substr(firstSep + 1, secSep - firstSep - 1);
	std::string version = line.substr(secSep + 1);
	
	if (method == "GET")
		this->_method = GET;
	else if (method == "POST")
		this->_method = POST;
	else if (method == "DELETE")
		this->_method = DELETE;
	else
		this->_method = UNKNOWN;
	
	this->_uri = uri;

	if (version == "HTTP/1.0")
		this->_version = HTTP_1_0;
	else if (version == "HTTP/1.1")
		this->_version = HTTP_1_1;
	else if (version == "HTTP/2")
		this->_version = HTTP_2;
	else
		this->_version = INVALID;
}
void Http::parseHeaders(const std::string &headers)
{
	//split line by line
	//find colon first?
	//key = str[0], colon pos
	//value = colonpos+2, /r/n?
	//save to header [key] = value
	//important save in class - contentlen cast to int
}

void Http::parseRequest(std::string rawString) 
{
	int bodyStart = 0;
	int separator = rawString.find("\r\n\r\n");
	if (!separator)
		throw HttpException(400);
	std::string requestLine = rawString.substr(0, rawString.find("\r\n"));
	std::string headers = rawString.substr(rawString.find("\r\n") + 2, separator - rawString.find("\r\n") - 2);
	parseRequestLine(requestLine);
	parseHeaders(headers);
	bodyStart = separator + 4;
	if (bodyStart < rawString.size())
	{
		this->_hasBody = true;
		std::string body = rawString.substr(bodyStart);
		this->setBody(body);
	}
	else
	{
		this->_hasBody = false;
	}
}

