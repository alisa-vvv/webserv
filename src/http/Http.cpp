#include <string>
#include <iostream>
#include "Http.hpp"
#include "httpBuffer.hpp"

int Http::validateURI(std::string uri)
{
	if (uri.empty() || uri.front() != '/')
	{
		setResponse(HTTP_BAD_REQUEST);
		return FAILURE;
	}
	if (uri.find("..") != std::string::npos || uri.find(" ") != std::string::npos || uri.find("//") != std::string::npos)
	{
		setResponse(HTTP_BAD_REQUEST);
		return FAILURE;
	}
	if (uri.length() > 2048)
	{
		setResponse(URI_TOO_LONG);
		return FAILURE;
	}
	return SUCCESS;
}
/// @brief validates the format/syntax of the request and sets the status code ONLY!
//It does not check for resource existence or permissions. That will be done in the controller layer.
void	Http::validateLayer()
{
	this->_type = RESPONSE;
	
	if (this->_version == INVALID)
	{
		setResponse(HTTP_VERSION_NOT_SUPPORTED);
		return;
	}
	if (this->_version == HTTP_1_0 && this->_method == POST)
	{
		setResponse(HTTP_VERSION_NOT_SUPPORTED);
		return;
	}
	if (this->_method == UNKNOWN)
	{
		setResponse(HTTP_METHOD_NOT_ALLOWED);
		return;
	}
	if (validateURI(this->_uri) == FAILURE)
		return;
	if (this->_headers.find("host") == this->_headers.end())
	{
		setResponse(HTTP_BAD_REQUEST);
		return;
	}
	if (this->_contentLen < 0 && this->_headers.find("content-length") != this->_headers.end())
	{
		setResponse(HTTP_BAD_REQUEST);
		return;
	}
	if (this->_contentLen > 1048576)
	{
		setResponse(PAYLOAD_TOO_LARGE); 
		return;
	}
	// GET/DELETE doesnt have body
	if ((this->_method == GET || this->_method == DELETE) && this->_hasBody)
	{
		setResponse(HTTP_BAD_REQUEST);
		return;
	}
	// POST needs content lenheader
	if (this->_method == POST && this->_headers.find("content-length") == this->_headers.end())
	{
		setResponse(HTTP_LENGTH_REQUIRED);
		return;
	}
	// POST with conlen should not have body
	if (this->_method == POST && this->_contentLen == 0 && this->_hasBody)
	{
		setResponse(HTTP_BAD_REQUEST);
		return;
	}	
	setResponse(HTTP_OK);
}



