#include "Http.hpp"
#include <fstream>

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
	//ticket04
	if (getState() != ERROR)
	{
		switch (this->_method)
		{
			case EXTENSION:
				setState(HANDLING_CGI_EXTENSION);
				return; //RETURN TO POLL LOOP, extension handler - should i set the headers?
				//in poll loop
				//if getmethod() == "EXTENSION"
				//fork exec blablabla
			case GET:
				return handleGetResponse();
			case POST:
				return handlePostResponse();
			case DELETE:
				return handleDeleteResponse();
		}
	}
	else
		return handleErrorResponse();
}