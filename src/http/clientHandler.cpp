#include "../../inc/Http.hpp"
#include "Colors.hpp"
#include "Client.hpp"

static int checkState(Http &http)
{
	if (http.getState() == CLIENT_ERROR)
	{
		http.buildResponse();
		http.printError("Built response in check state", http.getResponseString(), IS_VAR);
		return 0;
	}
	return 1;
}

/// @brief 
/// @param listener feed the listener struct
/// @param recvStr feed the str from recv
/// @return response string ready for accept

void	Client::clientHandler(std::string recvStr)
{
	Http &http = getHttpClass();
	std::cout << GREEN << "==================REQUEST START======================" << RESET << std::endl;
	
	http.parseRequest(recvStr);

	if (!checkState(http))
	{
		setResponse(http.getResponseString());
		http.printError("parseRequest", "clientHandler", NOT_VAR);
		std::cout << PURPLE << "==================REQUEST END======================" << RESET << std::endl;
		return;
	}

	http.setRequestConfig(getListenerClass());

	if (!checkState(http)) {
		setResponse(http.getResponseString());
		http.printError("Set Request Config", "clientHandler", NOT_VAR);
		return;
		std::cout << PURPLE << "==================REQUEST END======================" << RESET << std::endl;
	}

	http.validateLayer();

	if (!checkState(http)){
		setResponse(http.getResponseString());
		http.printError("validateLayer", "clientHandler", NOT_VAR);
		std::cout << PURPLE << "==================REQUEST END======================" << RESET << std::endl;
		return;
	}

	if (http.getState() == HANDLING_CGI_EXTENSION)
	{
		http.setState(HANDLING_CGI_EXTENSION);
		setResponse(http.getResponseString());
		return;
	}

	http.buildResponse();
	if (!checkState(http))
	{
		setResponse(http.getResponseString());
		http.printError("buildResponse", "clientHandler", NOT_VAR);
		std::cout << PURPLE << "==================REQUEST END======================" << RESET << std::endl;
		return;
	}
	setResponse(http.getResponseString());

	return;
	
}
