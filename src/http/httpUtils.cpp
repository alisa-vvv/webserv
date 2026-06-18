#include "Http.hpp"
#include <iostream>

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

