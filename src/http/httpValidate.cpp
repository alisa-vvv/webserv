#include <iostream>
#include <filesystem>
#include <sys/stat.h>
#include "../../inc/Http.hpp"

std::string Http::resolveUri(const std::string &uri)
{
	std::filesystem::path rootPath(uri);
	
	try
	{
		std::filesystem::path rootPath(uri);
		std::filesystem::path relativePath = std::filesystem::current_path() / rootPath.relative_path();
		return relativePath;
	}
	catch (const std::filesystem::filesystem_error &)
	{
	}
	return std::filesystem::path(uri).lexically_normal().string();
}

/// @brief Validate the file for permissions, file traversal, readable, deletable etc.
void Http::validateFile()
{
	std::string path = this->_builtUri;
	try
	{
		if (!std::filesystem::exists(path))
		{
			printError("file does not exist", "validateFile", NOT_VAR);
			return setResponseCode(HTTP_NOT_FOUND); //it dont exist
		}
		if (std::filesystem::is_directory(path)){
			if (this->_method == GET) //if its a directory, its ok for get but not for other methods
				return;
			printError("isDirectory", "validateFile", NOT_VAR);
			return setResponseCode(HTTP_NOT_FOUND);
		}
		if (!std::filesystem::is_regular_file(path)) {
			printError("regular file", "validateFile", NOT_VAR);
			return setResponseCode(HTTP_NOT_FOUND); //not a regular file aka dir/link
		}

		struct stat fileStat;
		if (stat(path.c_str(), &fileStat) == -1){ 
			printError("fileStat", "validateFile", NOT_VAR);
			return setResponseCode(HTTP_FORBIDDEN); //denied on parent dir, file perm change during chec and statcall
		}

		if (this->_method == GET && !(fileStat.st_mode & S_IRUSR))
		{
			printError("methodGET permission", "validateFile", NOT_VAR);
			return setResponseCode(HTTP_FORBIDDEN); //no read perm on get
		}
		else if (this->_method == DELETE && !(fileStat.st_mode & S_IWUSR)) {
			printError("method DEl permission", "validateFile", NOT_VAR);
			return setResponseCode(HTTP_FORBIDDEN); //no edit perm on del
		}
		else if (getExtension() == true && !(fileStat.st_mode & S_IXUSR)) {
			printError("extension permission", "validateFile", NOT_VAR);
			return setResponseCode(HTTP_FORBIDDEN); 
		}
	}
	catch (const std::filesystem::filesystem_error &e)
	{
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR); //internal server error
	}
}

void Http::buildAbsoluteUri()
{
	std::string prefix = requestConfig.location->prefix;
	std::string root = requestConfig.location->root;

	// allow config roots like /www to resolve inside the project when no real filesystem root exists.
	root = resolveUri(root);

	printError("root", root, IS_VAR);
	
	// remove prefix and add root
	std::string remaining = "/" + this->_receivedUri.substr(prefix.length());

	printError("remaining", remaining, IS_VAR);
	// only serve index if request ends with / or is exactly /
	bool isDirectoryRequest = (this->_receivedUri == "/") || (this->_receivedUri.length() > 0 && this->_receivedUri.back() == '/');

	if (isDirectoryRequest)
	{
		if (!requestConfig.location->index.empty())
		{
			if (remaining.empty())
			{
				this->_builtUri = root + "/" + requestConfig.location->index;
				printError("builturi1", getBuiltUri(), IS_VAR);
			}
			else if (remaining.back() == '/') {
				this->_builtUri = root + remaining + requestConfig.location->index;
				printError("builturi2", getBuiltUri(), IS_VAR);
			}
			else {
				this->_builtUri = root + remaining + "/" + requestConfig.location->index;
				printError("builturi3", getBuiltUri(), IS_VAR);		
			}
		}
		else {
			this->_builtUri = root + (remaining.empty() ? "/" : remaining);
			printError("builturi4", getBuiltUri(), IS_VAR);
		}
	}
	else
	{
		this->_builtUri = root + remaining;
		printError("builturi5", getBuiltUri(), IS_VAR);
	}
}

int Http::validateURI(std::string uri)
{
	// validate format: must start with /
	if (uri.empty() || uri.front() != '/')
	{
		printError("uri empty", "validateURI", NOT_VAR);
		setResponseCode(HTTP_BAD_REQUEST);
		return FAILURE;
	}
	// check for  path traversal, spaces, double slashes
	if (uri.find("..") != std::string::npos || uri.find(" ") != std::string::npos || uri.find("//") != std::string::npos)
	{
		printError("file traversal", "validateURI", NOT_VAR);
		setResponseCode(HTTP_BAD_REQUEST);
		return FAILURE;
	}
	// check length limit
	if (uri.length() > 2048)
	{
		printError("uri length", "validateURI", NOT_VAR);
		setResponseCode(URI_TOO_LONG);
		return FAILURE;
	}

	// check CGI extensions
	if (uri.find(".py") != std::string::npos || uri.find(".php") != std::string::npos || uri.find(".cgi") != std::string::npos)
		setExtension(true);
	
	// rewrite URI, strip location prefix and add root
	if (!requestConfig.location)
	{
		printError("no locations on requestConfig", "validateURI", NOT_VAR);
		return FAILURE;
	}
	buildAbsoluteUri();
	return SUCCESS;
}

/// @brief validates the format, syntax, and permissions
void	Http::validateLayer()
{
	setState(VALIDATING);
	if (this->_version == INVALID)
	{
		printError("Version", "validateLayer", NOT_VAR);
		return setResponseCode(HTTP_VERSION_NOT_SUPPORTED);
	}

	/*====HEADERS=====*/
	//content type if missing, if empty, 
	//if incorrectly set to URL-encoded form data when the content is in the request body instead 
	if (this->_requestHeaders.find("host") == this->_requestHeaders.end()) //missing host request error
	{
		printError("Missing host", "validateLayer", NOT_VAR);
		return setResponseCode(HTTP_BAD_REQUEST);
	}
	if (getHeader("host").empty()) //empty host string
	{
		printError("Host empty", "validateLayer", NOT_VAR);
		return setResponseCode(HTTP_BAD_REQUEST);
	}

	/*====METHODS=====*/
	
	if (this->_method == UNKNOWN)
	{
		printError("Method unknown", "validateLayer", NOT_VAR);
		return setResponseCode(HTTP_METHOD_NOT_ALLOWED);
	}

	else if (this->_method == GET)
	{
		if (requestConfig.location->allowed_methods.at((httpMethod)GET) == false) {
			printError("Method Get", "validateLayer", NOT_VAR);
			return setResponseCode(HTTP_METHOD_NOT_ALLOWED);
		}
	}

	else if (this->_method == POST)
	{
		if (requestConfig.location->allowed_methods.at((httpMethod)POST) == false) {
			printError("Post method false", "validateLayer", NOT_VAR);
			return setResponseCode(HTTP_METHOD_NOT_ALLOWED);
		}
		std::string contentLenStr = getHeader("content-length");
		if (contentLenStr.empty())
			return setResponseCode(HTTP_LENGTH_REQUIRED);
		try {
			int contentLen = std::stoi(contentLenStr);
			if (contentLen > (int)getClientMaxBodySize())
				return setResponseCode(HTTP_PAYLOAD_TOO_LARGE);
		}
		catch(std::exception &e){
			return setResponseCode(HTTP_BAD_REQUEST);
		}
		std::map<std::string, std::string>::iterator it = this->_requestHeaders.find("content-type");
		if (it == _requestHeaders.end())
		{
			printError("No content type", "validateLayer", NOT_VAR);
			return setResponseCode(HTTP_BAD_REQUEST);
		}
		else if (it->second.empty())
		{
			printError("Empty content type", "validateLayer", NOT_VAR);
			return setResponseCode(HTTP_BAD_REQUEST);
		}
	}

	else if (this->_method == DELETE)
	{ 
		if (requestConfig.location->allowed_methods.at(httpMethod(DELETE)) == false)
		{
			printError("Delete not allowed", "validateLayer", NOT_VAR);
			return setResponseCode(HTTP_METHOD_NOT_ALLOWED);
		}
	}

	/*=====URI=====*/
	if (validateURI(this->_receivedUri) == FAILURE)
		return;
	else
		validateFile();
}



