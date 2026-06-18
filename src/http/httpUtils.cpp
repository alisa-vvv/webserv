#include "Http.hpp"
#include <iostream>

Http::Http()
{
	
}

/// @brief moved to hpputils because it's kinda useless right now
/// @param httpObject 
/// @return 
std::string handleHttpResponse(Http &httpObject) //must take socket as param
{
	(void)httpObject;
	// httpObject.setResponse();
	// return(httpObject.getResponseString());
	// send();//send to socket
	return("");
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

