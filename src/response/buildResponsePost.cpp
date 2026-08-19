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
	std::string delim = "--" + getDelimiter(contentTypeString);
	std::string body = getBody();

	size_t start = body.find(delim);
	if (start == std::string::npos)
		return "";
	start += delim.size() + 2;
	
	size_t headerEnd = body.find("\r\n\r\n", start);
	if (headerEnd == std::string::npos)
		return "";
	
	std::string headers = body.substr(start, headerEnd - start);

	std::string key = "Content-Type: ";
	size_t pos = headers.find(key);
	if (pos == std::string::npos)
		return "";
	pos += key.size();
	size_t endVal = headers.find("\r\n", pos);
	std::string val = headers.substr(pos, endVal - pos);

	size_t contentStart = headerEnd + 4; // skip \r\n\r\n
	size_t contentEnd = body.find("\r\n--", contentStart);
	if (contentEnd == std::string::npos)
		contentEnd = body.size();
	
	std::string fileContent = body.substr(contentStart, contentEnd - contentStart);
	setRawBody(fileContent);
	return val;
}

std::string Http::resolveExtension()
{
	std::string contentTypeVal = getHeader("content-type");
	if (contentTypeVal.find("multipart/form-data") == std::string::npos) {
		setResponseCode(HTTP_UNSUPPORTED_MEDIA);
		return "";
	}
	std::string postContentTypeVal = getPostContentTypeVal(contentTypeVal);
	std::string extension = getContentTypeExtension(postContentTypeVal);

	if (extension.empty())
	{
		printError("Extension Empty", "handlePostResponse", NOT_VAR);
		setResponseCode(HTTP_UNSUPPORTED_MEDIA);
		return "";
	}
	return extension;
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
