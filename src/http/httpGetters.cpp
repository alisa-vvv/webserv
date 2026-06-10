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
	
}

std::string Http::getHeader(const std::string &key) const {
	
}

std::string Http::getUri() const {
	return (_uri);
}

httpVersion Http::getVersion() const {
	return (_version);
}

int Http::getStatusCode() const {
	
}

std::string Http::getResponseString() { //back to raw string for response

}