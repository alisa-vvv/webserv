#include "Http.hpp"
#include <map>

/// @brief 
/// @param listener 
/// @return 
int Http::findRequestConfig(Listener *listener)
{
	int configSize = listener->getAllConfig().size();
	std::string host = getHeader("host");
	size_t colonPos = host.find(":");
	if (colonPos != std::string::npos)
		host = host.substr(0, colonPos);
	for (int i = 0; i < configSize ; i++ )
	{
		if (host == listener->getServerConfig(i)->server_name)
			return i;
	}
	return -1;
	
}

/// @brief this function sets references to the matching configuration of the current client. it also rewrites the uri
void Http::setRequestConfig(Listener *listener)
{
	int ind = findRequestConfig(listener);
	if (ind == -1)
		ind = 0; //first server as default if no config match? ticket13
	else
	{
		this->requestConfig->server = listener->getServerConfig(ind);
		
	}
}
