#include <string>
#include <iostream>
#include "../../inc/Http.hpp"

/*======CONSTRUCTOR======*/

Http::Http()
	: _state(PARSING)
	, _method(UNKNOWN)
	, _version(INVALID)
	, _statusCode(0)
	, _contentLen(0)
	, _hasBody(false)
	, _hasExtension(false)
{}

/*======GETTERS======*/

clientState Http::getState() const {
	return (_state);
}

httpMethod Http::getMethod() const {
	return (_method);
}

int Http::getContentLen() const {
	return (_contentLen);
}

std::string Http::getBody() const {
	return (_body);
}

std::string Http::getHeader(const std::string &key) const {
	auto it = this->_requestHeaders.find(key);
	if (it != this->_requestHeaders.end())
		return (it->second);
	return "";
}

std::string Http::getBuiltUri() const {
	return (_builtUri);
}

std::string Http::getReceivedUri() const {
	return (_receivedUri);
}

httpVersion Http::getVersion() const {
	return (_version);
}

int Http::getStatusCode() const {
	return (_statusCode);
}

/// @brief get the response string ready for client use
/// @return 
std::string Http::getResponseString() const {
	return _responseString;
}

bool Http::getExtension() const {
	return (_hasExtension);
}

/*======SETTERS======*/

/// @brief Set the status code to given status. Also sets client state to error
//if status code!= OK
/// @param code 
void Http::setResponseCode(int code) {
	this->_statusCode = code;
	if (_statusCode != HTTP_OK && _statusCode != HTTP_CREATED)
		this->_state = CLIENT_ERROR;
}

void Http::setRequestHeader(const std::string &key, const std::string &value) {
	this->_requestHeaders.insert({key, value});
}

void Http::setResponseHeader(const std::string &key, const std::string &value) {
	this->_responseHeaders.insert({key, value});
}

void Http::setBody() {
	std::string file = this->_builtUri;
	std::ifstream fileStream(file, std::ios::binary);
	if (!fileStream.is_open())
		return setResponseCode(HTTP_FORBIDDEN);
	std::string body((std::istreambuf_iterator<char>(fileStream)),
			std::istreambuf_iterator<char>());
	this->_body = body;
}

void Http::setBody(const std::string uri) {
	if (uri.empty())
		this->_body = "";
	else
	{
		std::ifstream fileStream(uri, std::ios::binary);
		if (!fileStream.is_open())
		{
			this->_body = "";
			return;
		}
		std::string body((std::istreambuf_iterator<char>(fileStream)),
				std::istreambuf_iterator<char>());
		this->_body = body;
	}
}

void Http::setState(clientState state) {
	this->_state = state;
}

void Http::setExtension(bool status) {
	this->_hasExtension = status;
}

/*======UTILS======*/


void Http::debugPrintRequest()
{
	std::cout << "=== HTTP Debug Print ===" << std::endl;
	std::cout << "Type: " << this->getState() << std::endl;
	std::cout << "Method: " << this->getMethod() << std::endl;
	std::cout << "Version: " << this->getVersion() << std::endl;
	std::cout << "Status Code: " << this->getStatusCode() << std::endl;
	std::cout << "Content Length: " << this->getContentLen() << std::endl;
	std::cout << "Body: " << this->getBody() << std::endl;
	std::cout << "========================" << std::endl;
	
}

void Http::debugPrintRequestConfig()
{
	cfg_server_t server = requestConfig->server;
	t_location location = requestConfig->location;
	size_t serverNameCount = server->server_names.size();
	std::cout << "=== Context Request Config ====" << std::endl;
	std::cout << "============ SERVER ===========" << std::endl;
	std::cout << std::endl << "===============================" << std::endl;
	for (size_t i = 0; i < serverNameCount; i++)
		std::cout << "Server names: " << this->requestConfig.server->server_names[i] << std::endl;
	std::cout << "IP Address: " << server.ip_addr << std::endl;
	for (size_t i = 0; i < server.ports.size(); i++)
		std::cout << "Port: " << server.ports[i] << std::endl;
	std::cout << "Root: " << server.root << std::endl;
	std::cout << "Client max body: " << server.client_max_body_size << std::endl;
	std::cout << "Autoindex: " << server.autoindex << std::endl;
	for (const auto& [code, link] : server.error_pages)
		std::cout << "Error page status: " << code << std::endl << "Error page link: " << link << std::endl;
	std::cout << std::endl << "===============================" << std::endl << std::endl;

	std::cout << "============ LOCATION ===========" << std::endl;

	std::cout << "Prefix: " << location.prefix << std::endl;
	std::cout << "Root: " << location.root << std::endl;
	std::cout << "Index: " << location.index << std::endl;
	std::cout << "Autoindex: " << location.autoindex << std::endl;
	
	for (const auto& [method, allowed] : location.allowed_methods)
		std::cout << "Method: " << method << ", Allowed: " << allowed << std::endl;
	
	std::cout << "Upload store: " << location.upload_store << std::endl;
	std::cout << "CGI Pass path: " << location.cgi_pass.path << std::endl;
	std::cout << "CGI Pass extension: " << location.cgi_pass.extension << std::endl;
	std::cout << "Return code: " << location.returns.code << std::endl;
	if (location.returns.code)
		std::cout << "Return target: " << location.returns.target << std::endl;
}


void Http::debugPrintHttpClassAttributes()
{
	std::cout << "State: " << getState() << std::endl;
	std::cout << "Method: " << getMethod() << std::endl;
	std::cout << "ContentLen: " << getContentLen() << std::endl;
	std::cout << "==Headers==" <<std::endl;
	for (const auto& [header, content] : _requestHeaders)
		std::cout << header ": " << content << std::endl;
	std::cout << "Body: " << getBody() << std::endl;
	std::cout << "Version: " << getVersion() << std::endl;
	std::cout << "Status code: " << getStatusCode() << std::endl;
	std::cout << "Extension: " << getExtension() << std::endl;
	std::cout << "ReceivedUri: " << getReceivedUri() << std::endl;
	std::cout << "BuiltUri: " << getBuiltUri() << std::endl;
	std::cout << "Content type: " << getContentTypeExtension() << std::endl;
}