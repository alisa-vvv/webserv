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
	//ticket03
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

	return SUCCESS;
}

/// @brief calls the setrequest config, validates the format, syntax, and permissions, ticket00
void	Http::validateLayer()
{
	if (getState() == ERROR)
		return;
	setState(VALIDATING);
	setRequestConfig(); //ticket12

	if (this->_version == INVALID)
		return setResponseCode(HTTP_VERSION_NOT_SUPPORTED);

	/*METHODS*/
	if (this->_method == UNKNOWN)
		return setResponseCode(HTTP_METHOD_NOT_ALLOWED);
	if (this->_method == GET && requestConfig->GET == false)
		return setResponseCode(HTTP_METHOD_NOT_ALLOWED);
	if (this->_method == POST && requestConfig->POST == false)
		return setResponseCode(HTTP_METHOD_NOT_ALLOWED);
	if (this->_method == DELETE && requestConfig->DEL == false)
		return setResponseCode(HTTP_METHOD_NOT_ALLOWED);

	/*URI*/
	if (validateURI(this->_uri) == FAILURE)
		return;

	/*HEADERS*/
	if (this->_requestHeaders.find("host") == this->_requestHeaders.end())
		setResponseCode(HTTP_BAD_REQUEST);
	else
		validateFile();
} //ticket 11



