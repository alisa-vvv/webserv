#include "Http.hpp"

httpType Http::getType() const {
	return (_type);
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
	auto it = this->_headers.find(key);
	if (it != this->_headers.end())
		return (it->second);
	return "";
}

std::string Http::getUri() const {
	return (_uri);
}

httpVersion Http::getVersion() const {
	return (_version);
}

int Http::getStatusCode() const {
	return (_statusCode);
}

// std::string Http::getResponseString() { //back to raw string for response
// 	return (_res)
// }