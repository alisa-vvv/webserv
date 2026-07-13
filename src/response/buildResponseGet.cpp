#include "../../inc/Http.hpp"


void Http::handleGetResponse()
{
	std::string file = this->_uri;
	try {
		long size = std::filesystem::file_size(file);
		int maxBodySize = requestConfig->server->client_max_body_size;
		if (maxBodySize >= 0 && size > maxBodySize)
			return setResponseCode(HTTP_PAYLOAD_TOO_LARGE);
	
		setBody();
		setResponseCode(HTTP_OK);
		setState(READY_TO_SEND);
	}
	catch (std::exception &e)
	{
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
		return;
	}
	setContentType();
}
