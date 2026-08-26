#ifndef HTTP_ERROR_HPP
#define HTTP_ERROR_HPP

#include "Http.hpp"

/*HTTP ERROR CODES*/
static const int HTTP_OK = 200; //ok!
static const int HTTP_CREATED = 201; //1xx 2xx 3xx
static const int HTTP_MOVED_PERMANENTLY = 301; //moved permanently
static const int HTTP_BAD_REQUEST = 400; //client error
static const int HTTP_UNAUTHORIZED = 401; //no valid credentials, no login, expired token, wrong password etc
static const int HTTP_FORBIDDEN = 403; //no permission to access resource
static const int HTTP_NOT_FOUND = 404; //resource not found
static const int HTTP_METHOD_NOT_ALLOWED = 405; //method not supported by resource
static const int HTTP_REQUEST_TIMEOUT = 408; //client takes too long to send request
static const int HTTP_LENGTH_REQUIRED = 411; //length required
static const int HTTP_PAYLOAD_TOO_LARGE = 413; //request body too large
static const int URI_TOO_LONG = 414; //URI too long
static const int HTTP_UNSUPPORTED_MEDIA = 415; //unsupported media type
static const int HTTP_INTERNAL_SERVER_ERROR = 500; //cgi process fails
static const int HTTP_GATEWAY_TIMEOUT = 504; //Gateway timeout
static const int HTTP_VERSION_NOT_SUPPORTED = 505; //HTTP Version not supported


static const std::map<int, std::tuple<std::string, std::string>> HTTP_STATUS_MESSAGE = {
	{200, {"OK", ""}},
	{201, {"Created", ""}},
	{300, {"Multiple Choices", ""}},
	{302, {"Found", ""}},
	{400, {"Bad request", "www/error_pages/400.html"}},
	{401, {"Unauthorized", "www/error_pages/401.html"}},
	{403, {"Forbidden", "www/error_pages/403.html"}},
	{404, {"Not Found", "www/error_pages/404.html"}},
	{405, {"Method Not Allowed", "www/error_pages/405.html"}},
	{408, {"Request Timeout", "www/error_pages/408.html"}},
	// {409, {"Conflict", "www/error_pages/409.html"}},
	{411, {"Length Required", "www/error_pages/411.html"}},
	{413, {"Payload Too Large", "www/error_pages/413.html"}},
	{414, {"URI Too Long", "www/error_pages/414.html"}},
	{415, {"Unsupported Media Type", "www/error_pages/415.html"}},
	{500, {"Internal Server Error", "www/error_pages/500.html"}},
	// {502, {"Bad Gateway", "www/error_pages/502.html"}},
	{504, {"Gateway Timeout", "www/error_pages/504.html"}},
	{505, {"HTTP Version not supported", "www/error_pages/505.html"}}
};


#endif
