#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include <map>

static const int SUCCESS = 0;
static const int FAILURE = -1;
static const int HTTP_OK = 200;
static const int HTTP_CREATED = 201;
static const int HTTP_BAD_REQUEST = 400;
static const int HTTP_UNAUTHORIZED = 401;
static const int HTTP_FORBIDDEN = 403;
static const int HTTP_NOT_FOUND = 404;
static const int HTTP_METHOD_NOT_ALLOWED = 405;
static const int HTTP_REQUEST_TIMEOUT = 408;
static const int HTTP_LENGTH_REQUIRED = 411; // Length Required
static const int HTTP_PAYLOAD_TOO_LARGE = 413;
static const int URI_TOO_LONG = 414;
static const int HTTP_INTERNAL_SERVER_ERROR = 500;
static const int HTTP_VERSION_NOT_SUPPORTED = 505;

enum httpType {
	REQUEST,
	RESPONSE
};

enum httpVersion {
	HTTP_1_0,
	HTTP_1_1,
	INVALID
};

static std::map<int, std::string> HTTP_STATUS_MESSAGE = {
	{200, "OK"},//ok!
	{201, "Created"},//1xx 2xx 3xx
	{400, "Bad request"},//client error
	{401, "Unauthorized"}, //no valid auth
	{403, "Forbidden"}, //no permission to access resource
	{405, "Method Not Allowed"}, //method not supported by resource
	{404, "Not Found"}, //resource not found
	{408, "Request Timeout"}, //client takes too long to send request
	{409, "Conflict"}, //client error--request conflicts with current state of resource
	{413, "Payload Too Large"}, //request body too large
	{414, "URI Too Long"}, //URI too long
	{415, "Unsupported Media Type"}, //unsupported media type in request body
	{500, "Internal Server Error"}, //cgi process fails
	{502, "Bad Gateway"}, //cgi process terminates unexpectedly
	{504, "Gateway Timeout"}, //cgi process takes too long
	{505, "HTTP Version not supported"}
}; //405 408 409 413 415

enum httpMethod {
	GET,
	POST,
	DELETE,
	UNKNOWN
};

class Http {
	private:
		httpType							_type;
		httpMethod							_method;
		httpVersion							_version;
		int									_statusCode;
		int									_contentLen = -1;
		bool								_hasBody;
		bool								_hasExtension;
		std::string							_uri;
		std::string							_body;
		std::map<std::string, std::string>	_headers;

	public:
		Http();
		void			parseRequest(const std::string &rawString);
		void			parseRequestLine(const std::string line);
		void			parseHeaders(const std::string &headers);
		
		int				validateURI(std::string uri);
		void			validateLayer();
		void			buildResponse(); //give statuscode, set header, set body
	
		httpType		getType() const;
		httpMethod		getMethod() const;
		int				getContentLen() const;
		std::string		getBody() const;
		std::string		getHeader(const std::string &key) const;
		std::string		getUri() const;
		httpVersion		getVersion() const;
		int				getStatusCode() const;
		// std::string		getResponseString(); //back to raw string for response

	
		void			setResponse(int code); 
		void			setHeader(const std::string &key, const std::string &value);
		void			setBody(const std::string &body);

		void			debugPrint();
		
	class HttpException : public std::exception {
		private:
			std::string message;
		public:
			HttpException(int code) {
				message = "HTTP Error " + std::to_string(code) + ": " 
				+ HTTP_STATUS_MESSAGE.at(code);
			}
			const char *what() const noexcept override {
				return message.c_str();
			}
	};
};

void handleHttpRequest(Http &httpObject);
std::string handleHttpResponse(Http &httpObject); //must take socket as param


#endif