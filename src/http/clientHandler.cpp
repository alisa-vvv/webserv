#include "Http.hpp"

#define CHECK_STATE(client) if (client.getState() == ERROR) {client.buildResponse(); return;}
#define CHECK_CGI(client) if (client.getState() == HANDLING_CGI_EXTENSION) { return;}

//Call recv and http buffer

/// @brief 
/// @param listener feed the listener struct
/// @param recvStr feed the str from recv
/// @return response string ready for accept

std::string clientHandler(Listener *listener, std::string recvStr)
{
	Http client;
	
	client.parseRequest(recvStr);
	CHECK_STATE(client);

	client.setRequestConfig(Listener *listener); //ticket04
	CHECK_STATE(client);

	client.validateLayer();
	CHECK_STATE(client);

	client.buildResponse();
	CHECK_CGI(client);

	return client.getResponseString();
}