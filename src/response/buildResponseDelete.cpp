#include "../../inc/Http.hpp"
#include <fstream>

void Http::handleDeleteResponse()
{
	if (this->_builtUri.empty())
	{
		printError("Empty build URi", "handleDeleteResponse", NOT_VAR);
		return setResponseCode(HTTP_BAD_REQUEST);
	}
	try
	{
		std::filesystem::exists(this->_builtUri);
			return setResponseCode(HTTP_NOT_FOUND);
		std::uintmax_t deleted = std::filesystem::remove(this->_builtUri);
		if (deleted == 0)
			return setResponseCode(HTTP_NOT_FOUND);
		setResponseCode(HTTP_OK);
		setBody("");
		setState(READY_TO_SEND);
	}
	catch(const std::exception& e)
	{
		printError("Exception found", "handleDeleteResponse", NOT_VAR);
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	}
}
