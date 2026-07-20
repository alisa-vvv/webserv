#include "../../inc/Http.hpp"
#include <ctime>


void Http::handlePostResponse()
{
	if (!this->_hasBody || this->_body.empty())
		return setResponseCode(HTTP_BAD_REQUEST); //post needs body

	const std::string &uploadDir = requestConfig.location->upload_store;
	if (uploadDir.empty())
		return setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	try
	{
		std::filesystem::path basePath(uploadDir);
		if (!std::filesystem::exists(basePath) || !std::filesystem::is_directory(basePath))
			return setResponseCode(HTTP_FORBIDDEN);
	}
	catch (std::exception &e)
	{
		return setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	}
	std::string extension = getContentTypeExtension(getHeader("Content-Type"));
	if (extension.empty())
		return setResponseCode(HTTP_UNSUPPORTED_MEDIA);
	
	if (_body.size() > (size_t)requestConfig.server->client_max_body_size)
		return setResponseCode(HTTP_PAYLOAD_TOO_LARGE);

	try
	{
		std::string baseFileName = "upload_" + std::to_string(std::time(nullptr));
		std::string filename = baseFileName + extension;
		std::filesystem::path filepath = std::filesystem::path(uploadDir)/filename;
		
		int count = 1;
		while(std::filesystem::exists(filepath))
		{
			filename = baseFileName + "_" + std::to_string(count) + extension;
			filepath = std::filesystem::path(uploadDir)/filename;
			count++;
		}

		std::ofstream outFile(filepath, std::ios::binary);
		if (!outFile.is_open())
			return setResponseCode(HTTP_FORBIDDEN);
		outFile.write(this->_body.c_str(), this->_body.size());
		outFile.close();
		setResponseCode(HTTP_CREATED);
		setResponseHeader("Location:", "/" + filename);
		setBody("");
		setState(READY_TO_SEND);
	}
	catch (const std::exception &e)
	{
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	}
}
