#include "Http.hpp"
#include <fstream>

void Http::handlePostResponse()
{
	setState(HANDLING_CGI_STATIC);
	
	//parse request body, validate data, if valid set status code to 200, else set status code to 400 bad request
	setState(READY_TO_SEND);
	
}
