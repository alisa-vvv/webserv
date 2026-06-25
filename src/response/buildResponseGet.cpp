#include "Http.hpp"
#include <fstream>


void Http::handleGetResponse()
{
	//if file exists and is readable, set body to file content, set content length, set status code to 200
	//else if file exists but is not readable, set status code to 403
	//else set status code to 404 not foiund
	
	setState(HANDLING_CGI_STATIC);
	std::string file = this->_uri;
	try {
		long size = std::filesystem::file_size(file);
		if (size > CLIENT_MAX_BODY_SIZE)
			return setResponseCode(HTTP_PAYLOAD_TOO_LARGE);
		setHeader("Content-Length", std::to_string(size));
		
	}
	catch (std::exception &e)
	{
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
		return;
	}
	setState(READY_TO_SEND);
}
