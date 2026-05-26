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

void handleHttpRequest(http &httpObject)
{
	recvHttpRequest();
	httpObject.parseRequest();
}

std::string handleHttpResponse(http &httpObject) //must take socket as param
{
	httpObject.setResponse();
	return(httpObject.getResponseString());
	send();//send to socket
}


int main() //add to main
{
	http	httpObject;
	try 
	{
		handleHttpRequest(httpObject); //must take socket as param
		handleHttpResponse(httpObject);
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}