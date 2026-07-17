#include "../../inc/Http.hpp"

static int checkState(Http client)
{
	if (client.getState() == CLIENT_ERROR)
	{
		client.buildResponse();
		return 0;
	}
	return 1;
}

static int checkCgi(Http client)
{
	if (client.getState() == HANDLING_CGI_EXTENSION)
	{
		//call CGI
		//call build response?
		return 0;
	}
	return 1;
}
/// @brief 
/// @param listener feed the listener struct
/// @param recvStr feed the str from recv
/// @return response string ready for accept

std::string	clientHandler(const Listener *listener, std::string recvStr)
{
	Http client;
	
	client.parseRequest(recvStr);
	if (!checkState(client))
		return 0;

	client.setRequestConfig(listener); //ticket04
	if (!checkState(client))
		return 0;

	client.validateLayer();
	if (!checkState(client))
		return 0;

	client.buildResponse();
	if (!checkCgi(client))
		return 0;

	return client.getResponseString();
}
