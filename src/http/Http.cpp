#include <string>
#include <iostream>
#include "../../inc/Http.hpp"

/*======CONSTRUCTOR======*/

Http::Http()
	: _state(START)
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

std::string Http::getQuery() const {
	return (_query);
}

httpVersion Http::getVersion() const {
	return (_version);
}

int Http::getStatusCode() const {
	return (_statusCode);
}

std::string Http::getResponseString() const {
	return _responseString;
}

bool Http::getExtension() const {
	return (_hasExtension);
}

std::uintmax_t	Http::getClientMaxBodySize(){
	return _clientMaxBodySize;
}

bool Http::requestConfigExists() const {
	return _isRequestConfigSet;
}

/*======SETTERS======*/

/// @brief Set the status code to given status. Also sets client state to error
//if status code!= OK
/// @param code 
void Http::setResponseCode(int code) {
	this->_statusCode = code;
	if (_statusCode >= 400)
		this->_state = CLIENT_ERROR;
}

void Http::setRequestHeader(const std::string &key, const std::string &value) {
	this->_requestHeaders.insert({key, value});
}

void Http::setResponseHeader(const std::string &key, const std::string &value) {
	this->_responseHeaders.insert({key, value});
}

//assign from built uri
void Http::setBody() {
	std::string file = this->_builtUri;
	std::error_code fileError;

	if (!std::filesystem::is_regular_file(file, fileError))
	{
		printError("setBody target is not a file", "setBody", NOT_VAR);
		return setResponseCode(HTTP_FORBIDDEN);
	}
	std::ifstream fileStream(file, std::ios::binary);
	if (!fileStream.is_open())
	{
		printError("setBody fails","setBody", NOT_VAR);
		return setResponseCode(HTTP_FORBIDDEN);
	}
	std::string body((std::istreambuf_iterator<char>(fileStream)),
			std::istreambuf_iterator<char>());
	this->_body = body;
}

//assign from filepath
void Http::setBody(const std::string uri) {
	std::string resolvedUri = resolveUri(uri);
	if (resolvedUri.empty())
	{
		printError("resolvedUri Empty", "setBody(std::string)", NOT_VAR);
		this->_body = "";
	}
	else
	{
		std::error_code fileError;
		if (!std::filesystem::is_regular_file(resolvedUri, fileError))
		{
			printError("error page target is not a file", "setBody", NOT_VAR);
			this->_body = std::get<1>(HTTP_STATUS_MESSAGE.at(this->_statusCode));
			return;
		}
		std::ifstream fileStream(resolvedUri, std::ios::binary);
		if (!fileStream.is_open())
		{
			printError("Cannot open", "setBody", NOT_VAR);
			this->_body = std::get<1>(HTTP_STATUS_MESSAGE.at(this->_statusCode));
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

void Http::setClientMaxBodySize(){
	int	max_body_size;

	if (requestConfig.location->client_max_body_size != -1)
		max_body_size = requestConfig.location->client_max_body_size;
	else
		max_body_size = requestConfig.server->client_max_body_size;
	this->_clientMaxBodySize = static_cast<std::uintmax_t>(max_body_size) * 1024u * 1024u;
}

/// @brief direct assignment
/// @param body 
void Http::setRawBody(const std::string &body) {
	this->_body = body;
}

void Http::setRequestConfigExists() {
	_isRequestConfigSet = true;
}

/*======UTILS======*/


void Http::debugPrintRequest()
{
	std::cout << "=== HTTP Debug Print ===" << std::endl;
	std::cout << "Type: " << this->getState() << std::endl;
	std::cout << "Method: " << this->getMethod() << std::endl;
	std::cout << "Status Code: " << this->getStatusCode() << std::endl;
	std::cout << "Content Length: " << this->getContentLen() << std::endl;
	std::cout << "URI: " << this->getReceivedUri() << std::endl;
	std::cout << "========================" << std::endl;
	
}

void Http::debugPrintRequestConfig()
{
	const cfg_server_t *server = requestConfig.server;
	const t_location *location = requestConfig.location;
	size_t serverNameCount = server->server_names.size();
	std::cout << GREEN << "=== START Context Request Config ====" << RESET << std::endl;
	std::cout << "============ SERVER ===========" << std::endl;
	std::cout << std::endl << "===============================" << std::endl;
	for (size_t i = 0; i < serverNameCount; i++)
		std::cout << "Server names: " << server->server_names[i] << std::endl;
	std::cout << "IP Address: " << server->ip_addr << std::endl;
	for (size_t i = 0; i < server->ports.size(); i++)
		std::cout << "Port: " << server->ports[i] << std::endl;
	std::cout << "Root: " << server->root << std::endl;
	std::cout << "Client max body: " << server->client_max_body_size << std::endl;
	std::cout << "Autoindex: " << server->autoindex << std::endl;
	for (const auto& [code, link] : server->error_pages)
		std::cout << "Error page status: " << code << std::endl << "Error page link: " << link << std::endl;
		
	std::cout << "============ LOCATION ===========" << std::endl;

	std::cout << "Prefix: " << location->prefix << std::endl;
	std::cout << "Root: " << location->root << std::endl;
	std::cout << "Index: " << location->index << std::endl;
	std::cout << "Autoindex: " << location->autoindex << std::endl;
	
	for (const auto& [method, allowed] : location->allowed_methods)
		std::cout << "Method: " << method << ", Allowed: " << allowed << std::endl;
	
	std::cout << "Upload store: " << location->upload_store << std::endl;
	std::cout << "CGI Pass path: " << location->cgi_pass.path << std::endl;
	std::cout << "CGI Pass extension: " << location->cgi_pass.extension << std::endl;
	std::cout << "Return code: " << location->returns.code << std::endl;
	if (location->returns.code)
		std::cout << "Return target: " << location->returns.target << std::endl;
	std::cout << GREEN << "=== FINISH Context Request Config ====" << RESET << std::endl;

}


void Http::debugPrintHttpClassAttributes()
{
	std::cout << "State: " << getState() << std::endl;
	std::cout << "Method: " << getMethod() << std::endl;
	std::cout << "ContentLen: " << getContentLen() << std::endl;
	std::cout << "==Headers==" <<std::endl;
	for (const auto& [header, content] : _requestHeaders)
		std::cout << header << ": " << content << std::endl;
	std::cout << "Body: " << getBody() << std::endl;
	std::cout << "Version: " << getVersion() << std::endl;
	std::cout << "Status code: " << getStatusCode() << std::endl;
	std::cout << "Extension: " << getExtension() << std::endl;
	std::cout << "ReceivedUri: " << getReceivedUri() << std::endl;
	std::cout << "BuiltUri: " << getBuiltUri() << std::endl;
	std::cout << "Content type: " << getContentTypeExtension(getHeader("content-type")) << std::endl;
}

/// @brief print errors, which we do not have in this bea
/// @param error or the variable name in string
/// @param functName or the variable
/// @param isVar 
void Http::printError(std::string error, std::string functName, bool isVar)
{
	if (DEBUG_HTTP) {
		if (isVar)
			std::cout << RED << "Printing variable " << error << ": " << RESET << functName << std::endl;
		else 
			std::cout << RED << "Error: " << error << " on function " << RESET << functName << std::endl;
		std::cout << PURPLE << "==================REQUEST END======================" << RESET << std::endl;
	}
}

