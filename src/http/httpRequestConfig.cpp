#include "Http.hpp"
#include <map>


/// @brief this function sets references to the matching configuration of the current client
void Http::setRequestConfig()
{
	//listener.port , host, ip
	//first compare the host -> if there is no server name, then use the first one as default

	//check first for the listener.port
	//compare to config file
	/*=============URI, ROOT, INDEX=============*/
	if (this->_uri == "/")
	{
		if (!requestConfig->index.empty())
		{
			if (!requestConfig->root.empty())
				this->_uri = requestConfig->root + requestConfig->index;
			else
				this->_uri = requestConfig->index;
		}
		else
		{
			if (!requestConfig->root.empty())
				this->_uri = requestConfig->root;
		}
	} //ticket11, check for auto index

	/*============ALLOWED METHODS===============*/
	//check the map, iterator ;
	
	// for (const auto& pair : cfg.location.allowed_methods)
	// {
	// 	if (GET == TRUE)
	//	get == true;
	// }	
	//ticket04 ->check for server.port.allowed_methods


}
