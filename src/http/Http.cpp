#include <string>
#include <iostream>
#include "Http.hpp"


std::string recvHttpRequest() //must take socket as param
{
	int		maxRequest = 8192;
	char	buffer[maxRequest] = {0};
	int		totalReceived = 0;
	
	return std::string(buffer);
}

void handleHttpRequest(Http &httpObject)
{
	// std::string rawString = recvHttpRequest();
	// httpObject.parseRequest(rawString);
	char *testRawString = "POST /api/users?active=true HTTP/1.1\r\nHost: api.example.com\r\nContent-Type: application/json\r\nContent-Length: 37\r\nConnection: keep-alive\r\nAuthorization: Bearer eyJhbGci...\r\n\r\n{"username": "ally", "active": true}";
	httpObject.parseRequest(testRawString);

}

std::string handleHttpResponse(Http &httpObject) //must take socket as param
{
	httpObject.setResponse();
	return(httpObject.getResponseString());
	send();//send to socket
}


