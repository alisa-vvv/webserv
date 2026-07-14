#include "Http.hpp"
#include <iostream>


int main()
{
	Http testObj;
	char *tRequestString = "POST /api/users?active=true HTTP/1.1\r\nHost: api.example.com\r\nContent-Type: application/json\r\nContent-Length: 37\r\nConnection: keep-alive\r\nAuthorization: Bearer eyJhbGci...\r\n\r\n{"username": "ally", "active": true}";
	
	testObj.parseRequest(tRequestString);
}

// int main() //add to webserv main
// {
// 	Http	httpObject;
// 	try 
// 	{
// 		handleHttpRequest(httpObject); //must take socket as param
// 		handleHttpResponse(httpObject);
// 	}
// 	catch (std::exception &e)
// 	{
// 		std::cout << e.what() << std::endl;
// 	}
// }