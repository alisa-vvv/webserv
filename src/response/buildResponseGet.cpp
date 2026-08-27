#include "../../inc/Http.hpp"


void Http::handleGetResponse()
{
	std::string file = this->_builtUri;
	try {
		//uintmax_t size = std::filesystem::file_size(file);
	
		//check how to convert maxbodysize to mb
		//if (size > getClientMaxBodySize())
		//{
		//	printError("PAYLOAD too large", "handleGetResponse", NOT_VAR);
		//	return setResponseCode(HTTP_PAYLOAD_TOO_LARGE);
		//}
		setBody();
		if (getState() == CLIENT_ERROR)
			return;
		setResponseCode(HTTP_OK);
		setState(READY_TO_SEND);
	}
	catch (std::exception &e)
	{
		printError("Exception found", "handleGetResponse", NOT_VAR);
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
		return;
	}
	setContentType();
}
