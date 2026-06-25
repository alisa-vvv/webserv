#include "Http.hpp"
#include <fstream>

void Http::handleGetResponse()
{
	std::string file = this->_uri;
	try {
		long size = std::filesystem::file_size(file);
		if (size > requestConfig->clientMaxBodySize)
			return setResponseCode(HTTP_PAYLOAD_TOO_LARGE);
		setRequestHeader("Content-Length", std::to_string(size));
		// setRequestHeader("Content-Type", std::to_string(size));
		std::ifstream fileStream(file, std::ios::binary);
		std::string body((std::istreambuf_iterator<char>(fileStream)), 
				std::istreambuf_iterator<char>());
		setBody(body);
		setResponseCode(200);
		setState(READY_TO_SEND);
	}
	catch (std::exception &e)
	{
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
		return;
	}
	
}
