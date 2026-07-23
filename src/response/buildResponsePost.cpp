#include "../../inc/Http.hpp"
#include <ctime>
#include <filesystem>

std::string Http::resolveUploadDir()
{
	const std::string uploadDir = requestConfig.location->upload_store;
	if (uploadDir.empty())
	{
		printError("No upload directory configured", "handlePostResponse", NOT_VAR);
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
		return "";
	}

	std::filesystem::path root(resolveUri(requestConfig.location->root));
	std::filesystem::path uploadPath(uploadDir);
	std::filesystem::path finalPath = root / uploadPath.relative_path();

	try
	{
		if (!std::filesystem::exists(finalPath) || !std::filesystem::is_directory(finalPath)) {
			setResponseCode(HTTP_FORBIDDEN);
			return "";
		}
	}
	catch (std::exception &e)
	{
		printError("Exception found", "handlePostResponse", NOT_VAR);
		std::cout << e.what() << std::endl;
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
		return "";
	}
	return finalPath.lexically_normal().string();
}

static std::string getDelimiter(std::string contentTypeString)
{
	std::string boundaryStr = "boundary=";
	size_t bPos = contentTypeString.find(boundaryStr);
	if (bPos == std::string::npos)
		return "";
	else
		return contentTypeString.substr(bPos + boundaryStr.size()); //start after boundary=
}

std::string Http::getPostContentTypeVal(std::string contentTypeString)
{
	//contentTypeString = multipart/form-data; boundary=

	std::string delim = getDelimiter(contentTypeString);
	//delim = ----WebKitFormBoundarySudB14LqAoMOl84V
	std::string body = getBody();
	

}

std::string Http::resolveExtension()
{
	std::string contentTypeVal = getHeader("content-type");
	if (contentTypeVal.find("multipart/form-data") == std::string::npos) {
		setResponseCode(HTTP_UNSUPPORTED_MEDIA);
		return "";
	}
	std::string postContentTypeVal = getPostContentTypeVal(contentTypeVal);

	// std::string extension = getContentTypeExtension(contentTypeVal);

	if (extension.empty())
	{
		printError("Extension Empty", "handlePostResponse", NOT_VAR);
		setResponseCode(HTTP_UNSUPPORTED_MEDIA);
		return "";
	}
	
}

void Http::createUploadFile(std::string uploadDir, std::string extension)
{
	try {
	std::string baseFileName = "upload_" + std::to_string(std::time(nullptr));
		std::string filename = baseFileName + extension;
		std::filesystem::path filepath = std::filesystem::path(uploadDir) / filename;
		
		int count = 1;
		while(std::filesystem::exists(filepath))
		{
			filename = baseFileName + "_" + std::to_string(count) + extension;
			filepath = std::filesystem::path(uploadDir) / filename;
			count++;
		}

		std::ofstream outFile(filepath, std::ios::binary);
		// if (!outFile.is_open())
		// {
		// 	printError("Cannot open", "handlePostResponse", NOT_VAR);
		// 	return setResponseCode(HTTP_FORBIDDEN);
		// }
		outFile.write(this->_body.c_str(), this->_body.size());
		outFile.close();
		setResponseCode(HTTP_CREATED);
		setResponseHeader("Location", "/" + filename);
		setRawBody("");
		setState(READY_TO_SEND);
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		printError("Exception found", "handlePostResponse", NOT_VAR);
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	}
}

void Http::handlePostResponse()
{
	debugPrintHttpClassAttributes();

	if (!this->_hasBody || this->_body.empty())
	{
		printError("Body empty", "handlePostResponse", NOT_VAR);
		return setResponseCode(HTTP_BAD_REQUEST); //post needs body
	}

	std::string uploadDir = resolveUploadDir();

	if (getState() == CLIENT_ERROR)
		return;

	std::string extension = resolveExtension();

	if (getState() == CLIENT_ERROR)
		return;

	createUploadFile(uploadDir, extension);

	if (getState() == CLIENT_ERROR)
		return;

	
}
