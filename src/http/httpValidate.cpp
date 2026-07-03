#include <iostream>
#include <filesystem>
#include <sys/stat.h>
#include "Http.hpp"

/// @brief Validate the file for permissions, file traversal, readable, deletable etc.
void Http::validateFile()
{
	std::string path = this->_uri;
	try
	{
		if (!std::filesystem::exists(path))
			return setResponseCode(HTTP_NOT_FOUND); //it dont exist
		if (getExtension() == false && !std::filesystem::is_regular_file(path))
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
	if (uri.empty() || uri.front() != '/')
	{
		setResponseCode(HTTP_BAD_REQUEST);
		return FAILURE;
	}
	if (uri.find("..") != std::string::npos || uri.find(" ") != std::string::npos || uri.find("//") != std::string::npos)
	{
		setResponseCode(HTTP_BAD_REQUEST);
		return FAILURE;
	}
	if (uri.length() > 2048)
	{
		setResponseCode(URI_TOO_LONG);
		return FAILURE;
	}
	if (uri.find(".py") != std::string::npos || uri.find(".php") != std::string::npos || uri.find(".cgi") != std::string::npos)
		setExtension(true);
	if (this->_uri == "/")
	{
		if (!requestContext->index.empty())
		{
			if (!requestContext->root.empty())
				this->_uri = requestContext->root + requestContext->index;
			else
				this->_uri = requestContext->index;
		}
		else
		{
			if (!requestContext->root.empty())
				this->_uri = requestContext->root;
		}
	}
	return SUCCESS;
}

/// @brief calls the setrequest config, validates the format, syntax, and permissions
void	Http::validateLayer()
{
	if (getState() == ERROR)
		return;
	setState(VALIDATING);
	setRequestContext(); //ticket04

	if (this->_version == INVALID)
		return setResponseCode(HTTP_VERSION_NOT_SUPPORTED);

	/*====HEADERS=====*/
	//content type if missing, if empty, 
	//if incorrectly set to URL-encoded form data when the content is in the request body instead 
	if (this->_requestHeaders.find("host") == this->_requestHeaders.end()) //missing host error
		return setResponseCode(HTTP_BAD_REQUEST);

	/*====METHODS=====*/
	
	if (this->_method == UNKNOWN)
		return setResponseCode(HTTP_METHOD_NOT_ALLOWED);

	else if (this->_method == GET)
	{
		if (requestContext->GET == false)
			return setResponseCode(HTTP_METHOD_NOT_ALLOWED);
	}

	else if (this->_method == POST)
	{
		if (requestContext->POST == false)
			return setResponseCode(HTTP_METHOD_NOT_ALLOWED);
		std::map<std::string, std::string>::iterator it = this->_requestHeaders.find("content-type");
		if (it == _requestHeaders.end())
			return setResponseCode(HTTP_UNSUPPORTED_MEDIA);
		else if (it->second.empty())
			return setResponseCode(HTTP_BAD_REQUEST);
	}

	else if (this->_method == DELETE)
	{ 
		if (requestContext->DEL == false)
			return setResponseCode(HTTP_METHOD_NOT_ALLOWED);
	}

	/*=====URI=====*/
	if (validateURI(this->_uri) == FAILURE)
		return;
	else
		validateFile();
} //ticket 11



