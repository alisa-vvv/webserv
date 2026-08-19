#include "../../inc/Http.hpp"
#include "../../inc/Listener.hpp"
#include <map>

/// @brief  looks for the host in given servers
/// @param listener 
/// @return returns the index of the config with host match. returns -1 if no match

int Http::findRequestConfig(const Listener *listener)
{
	std::string host = getHeader("host");
	size_t colonPos = host.find(":");
	if (colonPos != std::string::npos)
		host = host.substr(0, colonPos);

	for (size_t i = 0; i < listener->getConfigCount() ; i++ )
	{
		const cfg_server_t *server = listener->getServerConfig(i);
		
		bool portMatch = false;
		for (int p : server->ports)
		{
			if (p == listener->getPort())
			{
				portMatch = true;
				break;
			}
		}
	if (!portMatch)
		continue;
	for (const auto &name : listener->getServerConfig(i)->server_names)
	{
		if (host == name)
			return i;
	}
	}
	return -1;
}


/// @brief sets a pointer to the matching configuration of the current client. it also rewrites the uri

void Http::setRequestConfig(const Listener *listener)
{
	if (listener->getConfigCount() <= 0)
		return setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	int ind = findRequestConfig(listener);
	if (ind == -1)
		ind = 0; //first server as default if no config match? ticket13

	const cfg_server_t *server = listener->getServerConfig(ind);
	if (!server)
		return setResponseCode(HTTP_INTERNAL_SERVER_ERROR);

	requestConfig.server = server;
	
	if (server->locations.empty())
		return setResponseCode(HTTP_INTERNAL_SERVER_ERROR);

	const t_location *bestMatch = nullptr;
	size_t longest = 0;
	for (const auto &currentLoc : server->locations)
	{
		if (currentLoc.prefix.empty())
		{
			bestMatch = &currentLoc;
			continue;
		}
		if (this->_receivedUri.find(currentLoc.prefix) == 0)
		{
			if (currentLoc.prefix.length() > longest)
			{
				bestMatch = &currentLoc;
				longest = currentLoc.prefix.length();
			}
		}
	}
	if (!bestMatch)
		bestMatch = &server->locations[0];
	requestConfig.location = bestMatch;
	setClientMaxBodySize();
}
