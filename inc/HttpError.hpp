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
static const int HTTP_VERSION_NOT_SUPPORTED = 505; //HTTP Version not supported


std::map<int, std::tuple<std::string, std::string>> HTTP_STATUS_MESSAGE = {
	{200, {"OK", ""}},
	{201, {"Created", ""}},
	{400, {"Bad request", "/errors/400.html"}},
	{401, {"Unauthorized", "/errors/401.html"}},
	{403, {"Forbidden", "/errors/402.html"}},
	{404, {"Not Found", "/errors/404.html"}},
	{405, {"Method Not Allowed", "/errors/405.html"}},
	{408, {"Request Timeout", "/errors/408.html"}},
	{409, {"Conflict", "/errors/409.html"}},
	{413, {"Payload Too Large", "/errors/413.html"}},
	{414, {"URI Too Long", "/errors/414.html"}},
	{415, {"Unsupported Media Type", "/errors/415.html"}},
	{500, {"Internal Server Error", "/errors/500.html"}},
	{502, {"Bad Gateway", "/errors/502.html"}},
	{504, {"Gateway Timeout", "/errors/504.html"}},
	{505, {"HTTP Version not supported", "/errors/505.html"}}
};


#endif