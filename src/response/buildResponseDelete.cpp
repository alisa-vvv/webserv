#include "../../inc/Http.hpp"
#include <fstream>
#include <system_error>

void Http::handleDeleteResponse()
{
	if (this->_builtUri.empty())
	{
		printError("Empty build URi", "handleDeleteResponse", NOT_VAR);
		return setResponseCode(HTTP_BAD_REQUEST);
	}
	std::error_code error;
	if (!std::filesystem::exists(this->_builtUri, error)) {
		if (error == std::errc::permission_denied)
			return setResponseCode(HTTP_FORBIDDEN);
		return setResponseCode(HTTP_NOT_FOUND);
	}

	bool deleted = std::filesystem::remove(this->_builtUri, error);
	if (error == std::errc::permission_denied)
		return setResponseCode(HTTP_FORBIDDEN);
	if (error)
	{
		printError(error.message(), "handleDeleteResponse", NOT_VAR);
		return setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	}
	if (!deleted)
		return setResponseCode(HTTP_NOT_FOUND);

	setResponseCode(HTTP_OK);
	setState(READY_TO_SEND);
}
