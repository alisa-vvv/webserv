#include "../../inc/Http.hpp"

/// @brief After building the response, build the response string ready for send()
/// @return response string 
void Http::buildResponseString()
{
	this->_responseString.clear();
	this->_responseString += "HTTP/1.0 " + std::to_string(this->_statusCode) + " "
		+ std::get<0>(HTTP_STATUS_MESSAGE.at(this->_statusCode)) + "\r\n";

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
	if (getState() != CLIENT_ERROR)
	{
		if (requestConfig->location->returns.code != 0)
			handleReturnResponse();
		else
		{
			if (getExtension() == true)
			{
				setState(HANDLING_CGI_EXTENSION);
				return;
			}
			setState(HANDLING_CGI_STATIC);
			if (this->_method == GET)
			{
				if (std::filesystem::is_directory(this->_uri))
				{
					if (requestConfig->location && requestConfig->location->autoindex)
						handleAutoIndexResponse();
					else if (requestConfig->server->autoindex)
						handleAutoIndexResponse();
					else
						setResponseCode(HTTP_NOT_FOUND);
				}
				else
					handleGetResponse();
			}
			else if (this->_method == POST)
				handlePostResponse();
			else if (this->_method == DELETE)
				handleDeleteResponse();
			else
				handleErrorResponse();
		}
	}
	if (getState() == CLIENT_ERROR)
		handleErrorResponse();
	setResponseHeader("Content-Length", std::to_string(this->_body.size()));
	setResponseHeader("Connection:", "keep-alive"); //ticket16
	buildResponseString();
}