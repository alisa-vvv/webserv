#include <iostream>
#include <filesystem>
#include <sys/stat.h>
#include "../../inc/Http.hpp"

/// @brief Validate the file for permissions, file traversal, readable, deletable etc.
void Http::validateFile()
{
	std::string path = this->_builtUri;
	try
	{
		if (!std::filesystem::exists(path))
			return setResponseCode(HTTP_NOT_FOUND); //it dont exist
		if (std::filesystem::is_directory(path))
		{
			if (this->_method == GET) //if its a directory, its ok for get but not for other methods
				return;
			return setResponseCode(HTTP_NOT_FOUND);
		}
		if (!std::filesystem::is_regular_file(path))
			return setResponseCode(HTTP_NOT_FOUND); //not a regular file aka dir/link

		struct stat fileStat;
		if (stat(path.c_str(), &fileStat) == -1)
			return setResponseCode(HTTP_FORBIDDEN); //denied on parent dir, file perm change during chec and statcall

		if (this->_method == GET && !(fileStat.st_mode & S_IRUSR))
			return setResponseCode(HTTP_FORBIDDEN); //no read perm on get
		else if (this->_method == DELETE && !(fileStat.st_mode & S_IWUSR))
			return setResponseCode(HTTP_FORBIDDEN); //no edit perm on del
		else if (this->_method == POST && !(fileStat.st_mode & S_IWUSR))
			return setResponseCode(HTTP_FORBIDDEN); //no write perm on post
		else if (getExtension() == true && !(fileStat.st_mode & S_IXUSR))
			return setResponseCode(HTTP_FORBIDDEN); 
	}
	catch (const std::filesystem::filesystem_error &e)
	{
		setResponseCode(HTTP_INTERNAL_SERVER_ERROR); //internal server error
	}
}

int Http::validateURI(std::string uri)
{
	// validate format: must start with /
	if (uri.empty() || uri.front() != '/')
	{
		setResponseCode(HTTP_BAD_REQUEST);
		return FAILURE;
	}
	// check for  path traversal, spaces, double slashes
	if (uri.find("..") != std::string::npos || uri.find(" ") != std::string::npos || uri.find("//") != std::string::npos)
	{
		setResponseCode(HTTP_BAD_REQUEST);
		return FAILURE;
	}
	// check length limit
	if (uri.length() > 2048)
	{
		setResponseCode(URI_TOO_LONG);
		return FAILURE;
	}
	
	// check CGI extensions
	if (uri.find(".py") != std::string::npos || uri.find(".php") != std::string::npos || uri.find(".cgi") != std::string::npos)
		setExtension(true);
	
	// rewrite URI, strip location prefix and add root
	if (!requestConfig->location)
		return FAILURE;
	
	std::string prefix = requestConfig->location->prefix;
	std::string root = requestConfig->location->root;
	
	// remove prefix and add root
	std::string remaining = this->_receivedUri.substr(prefix.length());

	if (remaining.empty() || this->_receivedUri == "/")
	{
		if (!requestConfig->location->index.empty())
			this->_builtUri = root + "/" + requestConfig->location->index;
		else
			this->_builtUri = root + "/";
		// if no index configured, just use root 
	} //add variable for relative uri
	else
		this->_builtUri = root + remaining;
	
	return SUCCESS;
}

/// @brief validates the format, syntax, and permissions
void	Http::validateLayer()
{
	setState(VALIDATING);
	if (this->_version == INVALID)
		return setResponseCode(HTTP_VERSION_NOT_SUPPORTED);

	/*====HEADERS=====*/
	//content type if missing, if empty, 
	//if incorrectly set to URL-encoded form data when the content is in the request body instead 
	if (this->_requestHeaders.find("host") == this->_requestHeaders.end()) //missing host request error
		return setResponseCode(HTTP_BAD_REQUEST);
	if (getHeader("host").empty()) //empty host string
		return setResponseCode(HTTP_BAD_REQUEST);

	/*====METHODS=====*/
	
	if (this->_method == UNKNOWN)
		return setResponseCode(HTTP_METHOD_NOT_ALLOWED);

	else if (this->_method == GET)
	{
		if (requestConfig->location->allowed_methods.at((e_method)GET) == false)
			return setResponseCode(HTTP_METHOD_NOT_ALLOWED);
	}

	else if (this->_method == POST)
	{
		if (requestConfig->location->allowed_methods.at((e_method)POST) == false)
			return setResponseCode(HTTP_METHOD_NOT_ALLOWED);
		std::map<std::string, std::string>::iterator it = this->_requestHeaders.find("content-type");
		if (it == _requestHeaders.end())
			return setResponseCode(HTTP_UNSUPPORTED_MEDIA);
		else if (it->second.empty())
			return setResponseCode(HTTP_BAD_REQUEST);
	}

	else if (this->_method == DELETE)
	{ 
		if (requestConfig->location->allowed_methods.at(e_method(DELETE)) == false)
			return setResponseCode(HTTP_METHOD_NOT_ALLOWED);
	}

	/*=====URI=====*/
	if (validateURI(this->_builtUri) == FAILURE)
		return;
	else
		validateFile();
} //ticket 11



