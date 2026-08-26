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

	std::error_code fileError;
	std::filesystem::file_status fileStatus = std::filesystem::status(finalPath, fileError);
	if (fileError)
	{
		printError("Cannot access upload directory", "handlePostResponse", NOT_VAR);
		setResponseCode(fileError == std::errc::permission_denied
			? HTTP_FORBIDDEN : HTTP_INTERNAL_SERVER_ERROR);
		return "";
	}
	if (!std::filesystem::exists(fileStatus) || !std::filesystem::is_directory(fileStatus))
	{
		setResponseCode(HTTP_FORBIDDEN);
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
		std::error_code fileError;
		while (std::filesystem::exists(filepath, fileError))
		{
			filename = baseFileName + "_" + std::to_string(count) + extension;
			filepath = std::filesystem::path(uploadDir) / filename;
			count++;
		}
		if (fileError)
		{
			printError("Cannot access upload file", "handlePostResponse", NOT_VAR);
			setResponseCode(HTTP_FORBIDDEN);
			return;
		}

		std::ofstream outFile(filepath, std::ios::binary);
		if (!outFile.is_open())
		{
			printError("Cannot write upload file", "handlePostResponse", NOT_VAR);
			setResponseCode(HTTP_FORBIDDEN);
			return;
		}
		outFile.write(this->_body.c_str(), this->_body.size());
		if (!outFile)
		{
			printError("Cannot write upload file", "handlePostResponse", NOT_VAR);
			setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
			return;
		}
		outFile.close();
		setResponseCode(HTTP_CREATED);
		std::string location = requestConfig.location->upload_store;
		if (location.back() != '/')
			location += '/';
		setResponseHeader("Location", location + filename);
		setRawBody("");
		setState(READY_TO_SEND);
	}
	catch (const std::filesystem::filesystem_error &e)
	{
		printError("Exception found", "handlePostResponse", NOT_VAR);
		setResponseCode(e.code() == std::errc::permission_denied
			? HTTP_FORBIDDEN : HTTP_INTERNAL_SERVER_ERROR);
	}
}

void Http::handlePostResponse()
{
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
