#include "Http.hpp"
#include "string.h"
#include <iostream>

//tRequestString = "POST /api/users?active=true HTTP/1.1\r\nHost: api.example.com\r\nContent-Type: application/json\r\nContent-Length: 37\r\nConnection: keep-alive\r\nAuthorization: Bearer eyJhbGci...\r\n\r\n{"username": "ally", "active": true}";

/// @brief Parses the first line of the request AKA request line and stores it in the Http obj.
//Example: POST /api/users HTTP/1.1\
/// @param line 
void Http::parseRequestLine(const std::string line)
{
	size_t firstSep = line.find(' ');
	size_t	secSep = line.rfind(' ');

	if (firstSep == std::string::npos || secSep == std::string::npos)
		throw HttpException(400);
	std::string method = line.substr(0, firstSep);
	std::string uri = line.substr(firstSep + 1, secSep - firstSep - 1);
	std::string version = line.substr(secSep + 1);
	
	this->_type = REQUEST; //DEBUG TEST
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

std::string trim(const std::string &str)
{
	size_t pre = str.find_first_not_of(" \t\r\n");
	if (pre == std::string::npos)
		return "";
	size_t post = str.find_last_not_of(" \t\r\n");
	return (str.substr(pre, (post - pre + 1)));
}

/// @brief Parses the headers and stores them in the Http obj _headers.
/// @param headers 
void Http::parseHeaders(const std::string &headers)
{
	size_t start = 0;
	size_t colon = 0;
	size_t newLine = 0;
	std::string key, val, keyStr, valStr, currLine;
	
	while (start < headers.length())
	{
		newLine = headers.find("\r\n", start);
		if (newLine == std::string::npos)
			throw HttpException(400); //bad request
		colon = headers.find(":", start);
		if (colon == std::string::npos || colon > newLine)
			throw HttpException(400); //bad request
		currLine = headers.substr(start, newLine - start);
		// Recalculate colon and newLine positions relative to currLine
		size_t colonInLine = colon - start;
		keyStr = currLine.substr(0, colonInLine);
		valStr = currLine.substr(colonInLine + 1);
		keyStr = trim(keyStr);
		valStr = trim(valStr);
		key = "";
		val = "";
		for (unsigned long i = 0; i < keyStr.length(); i++)
			key += tolower(keyStr[i]);
		for (unsigned long i = 0; i < valStr.length(); i++)
			val += tolower(valStr[i]);
		if (key == "content-length")
		{
			try {
				this->_contentLen = stoi(val);
			}
			catch (std::exception &e){
				throw HttpException(400);
			}
		}
		this->_headers[key] = val;
		start = newLine + 2;
	}
}
/// @brief After receiving the rawstring, it gets parsed here.
/// @param rawString 
void Http::parseRequest(const std::string &rawString) 
{
	size_t	bodyStart = 0;
	size_t	separator = rawString.find("\r\n\r\n");
	if (separator == std::string::npos)
		throw HttpException(400);
	size_t requestLineEnd = rawString.find("\r\n");
	std::string requestLine = rawString.substr(0, rawString.find("\r\n"));
	std::string headers = rawString.substr(requestLineEnd + 2, separator - requestLineEnd);
	parseRequestLine(requestLine);
	parseHeaders(headers);
	bodyStart = separator + 4;
	if (bodyStart < rawString.size())
	{
		std::string body = rawString.substr(bodyStart);
		if ((unsigned long)this->_contentLen > 0 && body.size() != (unsigned long)this->_contentLen)
			throw HttpException(400);
		else if (this->_contentLen == 0 && body.size() > 0)
			throw HttpException(400); //conlen 0 but body exists
		this->_hasBody = true;
		this->setBody(body);
	}
	else
		this->_hasBody = false;
}

