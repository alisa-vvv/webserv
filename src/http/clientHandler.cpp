#include "../../inc/Http.hpp"
#include "Colors.hpp"

static int checkState(Http client)
{
	if (client.getState() == CLIENT_ERROR)
	{
		client.buildResponse();
		return 0;
	}
	return 1;
}

// static int checkCgi(Http &client)
// {
// 	if (client.getState() == HANDLING_CGI_EXTENSION)
// 	{
// 		//call CGI
// 		//call build response?
// 		return 0;
// 	}
// 	return 1;
// }


/// @brief 
/// @param listener feed the listener struct
/// @param recvStr feed the str from recv
/// @return response string ready for accept

std::string	clientHandler(const Listener *listener, std::string recvStr)
{
	std::cout << GREEN << "==================REQUEST START======================" << RESET << std::endl;
	Http client;
	
	client.parseRequest(recvStr);

	if (!checkState(client))
	{
		std::cout << RED << "Failed on parseRequest" << RESET << std::endl;
		return client.getResponseString();
	}

	client.setRequestConfig(listener);

	if (!checkState(client)) {
		std::cout << RED << "Failed on setrequestConfig" << RESET << std::endl;
		return client.getResponseString();
	}

	client.validateLayer();

	if (!checkState(client)){
		std::cout << RED << "Failed on validateLayer" << RESET << std::endl;
		return client.getResponseString();
	}

	// if (checkCgi(client))
	// {
	// 	sendToCgi(client);
	// }

	client.buildResponse();
	{
		std::cout << RED << "Failed on buildResponse" << RESET << std::endl;
		return client.getResponseString();
	}
	client.debugPrintHttpClassAttributes();
	client.debugPrintRequestConfig();

	return client.getResponseString();
	std::cout << PURPLE << "==================REQUEST END======================" << RESET << std::endl;
}
