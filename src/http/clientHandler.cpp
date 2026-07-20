#include "../../inc/Http.hpp"
#include "Colors.hpp"

<<<<<<< Updated upstream
#define CHECK_STATE(client) if (client.getState() == CLIENT_ERROR) {client.buildResponse(); return 0;}
#define CHECK_CGI(client) if (client.getState() == HANDLING_CGI_EXTENSION) { return 0;}

//Call recv and http buffer

=======
static int checkState(Http &client)
{
	if (client.getState() == CLIENT_ERROR)
	{
		client.buildResponse();
		return 0;
	}
	return 1;
}

// static int checkCgi(Http client)
// {
// 	if (client.getState() == HANDLING_CGI_EXTENSION)
// 	{
// 		//call CGI
// 		//call build response?
// 		return 0;
// 	}
// 	return 1;
// }
>>>>>>> Stashed changes
/// @brief 
/// @param listener feed the listener struct
/// @param recvStr feed the str from recv
/// @return response string ready for accept

std::string	clientHandler(const Listener *listener, std::string recvStr)
{
	std::cout << GREEN << "==================REQUEST START======================" << RESET << std::endl;
	Http client;
	
	client.parseRequest(recvStr);
<<<<<<< Updated upstream
	//CHECK_STATE(client);

	client.setRequestConfig(listener); //ticket04
	//CHECK_STATE(client);

	client.validateLayer();
	//CHECK_STATE(client);

	client.buildResponse();
	//CHECK_CGI(client);
=======
	if (!checkState(client))
	{
		std::cout << RED << "Parse request fail" << RESET << std::endl;
		return client.getResponseString();
	}

	client.setRequestConfig(listener);
	if (!checkState(client))
	{
		std::cout << RED << "Request config fail" << RESET << std::endl;
		return client.getResponseString();
	}

	client.debugPrintHttpClassAttributes();
	client.debugPrintRequestConfig();

	client.validateLayer();
	if (!checkState(client))
	{
		std::cout << RED << "Validate layer fail" << RESET << std::endl;
		return client.getResponseString();
	}
	
	client.buildResponse();
	// if (checkCgi(client))
	// 	return client.getResponseString();

>>>>>>> Stashed changes

	return client.getResponseString();

	std::cout << PURPLE << "==================REQUEST END======================" << RESET << std::endl;

}
