#include "../../inc/Http.hpp"
#include <fstream>

void Http::handleDeleteResponse()
{
	if (this->_builtUri.empty())
		return setResponseCode(HTTP_BAD_REQUEST);
	try
	{
		std::filesystem::remove(this->_builtUri);
		setResponseCode(HTTP_OK);
		setBody("");
		setState(READY_TO_SEND);
	}
	catch(const std::exception& e)
	{
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	}
}
