#include "../../inc/Http.hpp"
#include "Colors.hpp"

static int checkState(Http &client)
{
	if (client.getState() == CLIENT_ERROR)
	{
		client.buildResponse();
		client.printError("Built response in check state", client.getResponseString(), IS_VAR);
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
		client.printError("parseRequest", "clientHandler", NOT_VAR);
		std::cout << PURPLE << "==================REQUEST END======================" << RESET << std::endl;
		return client.getResponseString();
	}

	client.setRequestConfig(listener);

	if (!checkState(client)) {
		client.printError("Set Request Config", "clientHandler", NOT_VAR);
		return client.getResponseString();
		std::cout << PURPLE << "==================REQUEST END======================" << RESET << std::endl;
	}

	client.validateLayer();

	if (!checkState(client)){
		client.printError("validateLayer", "clientHandler", NOT_VAR);
		client.printError("response string", client.getResponseString(), IS_VAR);
		std::cout << PURPLE << "==================REQUEST END======================" << RESET << std::endl;
		return client.getResponseString();
	}

	// if (checkCgi(client))
	// {
	// 	sendToCgi(client);
	// }

	client.buildResponse();
	{
		client.printError("buildResponse", "clientHandler", NOT_VAR);
		std::cout << PURPLE << "==================REQUEST END======================" << RESET << std::endl;
		return client.getResponseString();
	}
	return client.getResponseString();
	std::cout << PURPLE << "==================REQUEST END======================" << RESET << std::endl;
}
