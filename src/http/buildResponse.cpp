#include "Http.hpp"
#include <fstream>


void Http::handleGet()
{
	//if file exists and is readable, set body to file content, set content length, set status code to 200
	//else if file exists but is not readable, set status code to 403
	//else set status code to 404 not foiund
	std::string file = this->_uri;
	try {
		long size = std::filesystem::file_size(file);
		if (size > CLIENT_MAX_BODY_SIZE)
			return setResponseCode(HTTP_PAYLOAD_TOO_LARGE);
		
	}
	
}

void Http::handlePost()
{
	//parse request body, validate data, if valid set status code to 200, else set status code to 400 bad request
}

void Http::handleDelete()
{
	//if file exists and is deletable, delete file, set status code to 200
	//else if file exists but is not deletable, set status code to 403
	//else set status code to 404 not foiund
}

/// @brief get the response string ready for client use
/// @return 
std::string Http::getResponseString() const
{
	return _responseString;
}

/// @brief After building the response, build the response string ready for send()
/// @return response string 
void Http::buildResponseString()
{
	this->_responseString += "HTTP/1.1 " + std::to_string(this->_statusCode) + " " 
		+ HTTP_STATUS_MESSAGE.at(this->_statusCode) + "\r\n";

	for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
	
	this->_responseString += it->first + ": " + it->second + "\r\n";	
	this->_responseString += "\r\n";
	this->_responseString += this->_body;
}


/// @brief After validating request and determining status code, build the response by setting headers 
//and body based on status code and request method
void Http::buildResponse()
{
	if (this->_statusCode == 200)
	{
		switch (this->_method)
		{
			case EXTENSION:
				return; //extension handler
			case GET:
				return handleGet();
			case POST:
				return handlePost();
			case DELETE:
				return handleDelete();
		}
	}
	else
		return loadErrorPage();
}