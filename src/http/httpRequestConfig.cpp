#include "Http.hpp"
#include <map>


/// @brief this function sets references to the matching configuration of the current client. it also rewrites the uri
void Http::setRequestConfig()
{
	/*===HOST, LOCATION population===*/
	//listener.port , host, ip
	//first compare the host -> if there is no server name, then use the first one as default	
	//check first for the listener.port
	//compare to config file
	//ticket11, check for auto index

	/*============ALLOWED METHODS===============*/
	//check the map, iterator ;
	// for (const auto& pair : cfg.location.allowed_methods)
	// {
	// 	if (GET == TRUE)
	//	get == true;
	// }	
	//ticket04 ->check for server.port.allowed_methods


}
