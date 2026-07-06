#include "Http.hpp"

/// @brief After building the response, build the response string ready for send()
/// @return response string 
void Http::buildResponseString()
{
	this->_responseString.clear();
	this->_responseString += "HTTP/1.0 " + std::to_string(this->_statusCode) + " "
		+ HTTP_STATUS_MESSAGE.at(this->_statusCode) + "\r\n";

	for (std::map<std::string, std::string>::iterator it = this->_responseHeaders.begin(); it != this->_responseHeaders.end(); ++it)
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
		if (getExtension() == true)
		{
			setState(HANDLING_CGI_EXTENSION);
			return;
		}
		setState(HANDLING_CGI_STATIC);
		if (this->_method == GET)
			handleGetResponse();
		else if (this->_method == POST)
			handlePostResponse();
		else if (this->_method == DELETE)
			handleDeleteResponse();

	}
	else
		handleErrorResponse();
	setContentType();
	setResponseHeader("Content-Length", std::to_string(this->_body.size()));
	buildResponseString();
}