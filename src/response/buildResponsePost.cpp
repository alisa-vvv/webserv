#include "../../inc/Http.hpp"
#include <ctime>
#include <filesystem>


void Http::handlePostResponse()
{
	if (!this->_hasBody || this->_body.empty())
	{
		printError("Body empty", "handlePostResponse", NOT_VAR);
		return setResponseCode(HTTP_BAD_REQUEST); //post needs body
	}

	const std::string &uploadDir = requestConfig.location->upload_store;
	if (uploadDir.empty())
	{
		printError("No upload directory configured", "handlePostResponse", NOT_VAR);
		return setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	}
	try
	{
		std::filesystem::path basePath(uploadDir);
		if (!std::filesystem::exists(basePath))
		{
			if (!std::filesystem::create_directory(uploadDir))
				return setResponseCode(HTTP_FORBIDDEN);
		}
	}
	catch (std::exception &e)
	{
		printError("Exception found", "handlePostResponse", NOT_VAR);
		return setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	}
	std::string extension = getContentTypeExtension(getHeader("content-type"));
	if (extension.empty())
	{
		printError("Extension Empty", "handlePostResponse", NOT_VAR);
		return setResponseCode(HTTP_UNSUPPORTED_MEDIA);
	}
	
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
		{
			printError("Cannot open", "handlePostResponse", NOT_VAR);
			return setResponseCode(HTTP_FORBIDDEN);
		}
		outFile.write(this->_body.c_str(), this->_body.size());
		outFile.close();
		setResponseCode(HTTP_CREATED);
		setResponseHeader("Location:", "/" + filename);
		setBody("");
		setState(READY_TO_SEND);
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		printError("Exception found", "handlePostResponse", NOT_VAR);
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	}
}
