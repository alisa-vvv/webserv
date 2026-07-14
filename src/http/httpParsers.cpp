#include "Http.hpp"


//tRequestString = "POST /api/users?active=true HTTP/1.1\r\nHost: api.example.com\r\nContent-Type: application/json\r\nContent-Length: 37\r\nConnection: keep-alive\r\nAuthorization: Bearer eyJhbGci...\r\n\r\n{"username": "ally", "active": true}";


void Http::parseRequestLine(const std::string line)
{
	//separate by space
	//first is method
		//validate and throw exception if UNKNOWN
	//second is uri
		//validate and throw exception if UNKNOWN
	//third is version
		//validate and throw exception if UNKNOWN
	
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

void Http::parseRequest(std::string rawString) {
	int separator = rawString.find("\r\n\r\n");
	if (!separator)
		throw HttpException(400);
	std::string requestLine = rawString.substr(0, rawString.find("\r\n"));
	std::string headers = rawString.substr(rawString.find("\r\n") + 2, separator - rawString.find("\r\n") - 2);
	
	parseRequestLine(requestLine);
	parseHeaders(headers);
}

