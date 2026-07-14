
#include <string>
#include <iostream>

int main()
{
	std::string uri = "/index";
	std::string root = "/home";
	//ticket03
	if (uri.empty() || uri.front() != '/')
	{
		std::cout << "empty or /" << std::endl;
		return 0;
	}
	if (uri.find("..") != std::string::npos || uri.find(" ") != std::string::npos || uri.find("//") != std::string::npos)
	{
		std::cout << "second error" << std::endl;
		return 0;
	}
	if (uri.length() > 2048)
	{
		std::cout << "too long" << std::endl;
		return 0;
	}
	if (uri.find(".py") != std::string::npos || uri.find(".php") != std::string::npos || uri.find(".cgi") != std::string::npos)
	{
		std::cout << "eextension" << std::endl;
		return 0;
	}
	std::string newUri = root + uri; //ticket11
	std::cout << newUri << std::endl;

	return 1;
}