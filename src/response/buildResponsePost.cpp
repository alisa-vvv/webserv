#include "../../inc/Http.hpp"
#include <fstream>

void Http::handlePostResponse()
{
	if (!this->_hasBody || this->_body.empty())
		return setResponseCode(HTTP_BAD_REQUEST); //post needs body
	const std::string &uploadDir = requestConfig->location->upload_store;
	if (uploadDir.empty())
		return setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	//create the file? 
	try
	{
		std::filesystem::path basePath(uploadDir);
		if (!std::filesystem::exists(basePath) || !std::filesystem::is_directory(basePath))
			return setResponseCode(HTTP_FORBIDDEN);
	}
	catch (std::exception &e)
	{
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	}
	//parse request body, validate data, if valid set status code to 200, else set status code to 400 bad request
	setState(READY_TO_SEND);
	setContentType(); 
}
