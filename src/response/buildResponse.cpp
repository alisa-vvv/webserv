#include "../../inc/Http.hpp"
#include <filesystem>

std::string Http::buildCGIResponseString(std::string cgiResponse)
{
	size_t firstSpace = cgiResponse.find(" ");
	size_t newLine = cgiResponse.find("\r\n"); 
	std::string startLine = cgiResponse.substr(0, firstSpace);
	std::string cgiResponseString;
	if (startLine != "HTTP/1.1" || startLine != "HTTP/1.0")
		cgiResponseString = cgiResponse;
	else
	{
		cgiResponseString += "HTTP/1.1 200 OK\r\n";
		cgiResponseString += cgiResponse.substr(newLine + 2);
	}
	return cgiResponseString;
}

/// @brief After building the response, build the response string ready for send()
/// @return response string 
void Http::buildResponseString()
{
	std::string httpVer = "HTTP/1.1 ";
	if (getVersion() == HTTP_1_0)
		httpVer = "HTTP/1.0 ";
	else if (getVersion() == HTTP_1_1)
		httpVer = "HTTP/1.1 ";
	this->_responseString.clear();
	this->_responseString += httpVer + std::to_string(this->_statusCode) + " "
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
	if (getState() != CLIENT_ERROR)
	{
		if (requestConfig.location->returns.code != 0)
			handleReturnResponse();
		else
		{
			setState(HANDLING_CGI_STATIC);
			if (this->_method == GET)
			{
				if (std::filesystem::is_directory(this->_builtUri))
				{
					if (requestConfig.location && requestConfig.location->autoindex)
						handleAutoIndexResponse();
					else if (requestConfig.server->autoindex)
						handleAutoIndexResponse();
					else
						setResponseCode(HTTP_FORBIDDEN);
				}
				else
					handleGetResponse();
			}
			else if (this->_method == POST)
				handlePostResponse();
			else if (this->_method == DELETE)
				handleDeleteResponse();
			else
				handleErrorResponse(); //defensive fallcase->getstate should already handle it
		}
	}
	if (getState() == CLIENT_ERROR) {
		printError("getStateError", "buildResponse", NOT_VAR);
		handleErrorResponse();
	}
	setResponseHeader("Content-Length", std::to_string(this->_body.size()));
	setResponseHeader("Connection", "keep-alive");
	// setResponseHeader("Date", time(nullptr)); need to add date and not time. probably need to make a httpdatefucntion
	buildResponseString();
	if (getState() == READY_TO_SEND)
		return ;
}
