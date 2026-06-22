#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include <map>

static const int SUCCESS = 0;
static const int FAILURE = -1;
static const int CLIENT_MAX_BODY_SIZE = 100; //we probably need to change this ticket01
static const int HTTP_OK = 200; //ok!
static const int HTTP_CREATED = 201; //1xx 2xx 3xx
static const int HTTP_BAD_REQUEST = 400; //client error
static const int HTTP_UNAUTHORIZED = 401; //no valid credentials, no login, expired token, wrong password etc
static const int HTTP_FORBIDDEN = 403; //no permission to access resource
static const int HTTP_NOT_FOUND = 404; //resource not found
static const int HTTP_METHOD_NOT_ALLOWED = 405; //method not supported by resource
static const int HTTP_REQUEST_TIMEOUT = 408; //client takes too long to send request
static const int HTTP_LENGTH_REQUIRED = 411; //length required
static const int HTTP_PAYLOAD_TOO_LARGE = 413; //request body too large
static const int URI_TOO_LONG = 414; //URI too long
static const int HTTP_INTERNAL_SERVER_ERROR = 500; //cgi process fails
static const int HTTP_VERSION_NOT_SUPPORTED = 505; //HTTP Version not supported

enum httpType {
	REQUEST,
	RESPONSE
};

enum httpVersion {
	HTTP_1_0,
	HTTP_1_1, //we will not handle this ticket08
	INVALID //we will not handle http versions other than 1.0 and 1.1, so we set INVALID for unsupported versions
};

static std::map<int, std::string> HTTP_STATUS_MESSAGE = {
	{200, "OK"},
	{201, "Created"},
	{400, "Bad request"},
	{401, "Unauthorized"},
	{403, "Forbidden"},
	{405, "Method Not Allowed"},
	{404, "Not Found"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{413, "Payload Too Large"},
	{414, "URI Too Long"},
	{415, "Unsupported Media Type"},
	{500, "Internal Server Error"},
	{502, "Bad Gateway"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version not supported"}
};

enum httpMethod {
	GET,
	POST,
	DELETE,
	EXTENSION, //not gonnna show up in config file
	UNKNOWN,
	UNALLOWED //use this for checking methods  config methods allowed
};

class Http {
	private:
		httpType							_type;
		httpMethod							_method;
		httpVersion							_version;
		int									_statusCode;
		int									_contentLen;
		bool								_hasBody;
		std::string							_uri;
		std::string							_body;
		std::string							_responseString;
		std::map<std::string, std::string>	_headers;

	public:
		Http();
		/*==========PARSING===========*/
		void			parseRequest(const std::string &rawString);
		void			parseRequestLine(const std::string line);
		void			parseHeaders(const std::string &headers);

		/*==========VALIDATION========*/
		int				validateURI(std::string uri);
		void			validateLayer();
		void			validateFile();
	
		/*==========GETTERS============*/
		httpType		getType() const;
		httpMethod		getMethod() const;
		int				getContentLen() const;
		std::string		getBody() const;
		std::string		getHeader(const std::string &key) const;
		std::string		getUri() const;
		httpVersion		getVersion() const;
		int				getStatusCode() const;
		std::string		getResponseString() const;

		/*==========METHOD HANDLERS==========*/
		void 			handleGet();
		void 			handlePost();
		void 			handleDelete();

		/*==========ERROR LOADER================*/
		void			loadErrorPage(); //load error page content into body based on status

		/*===========RESPONSE BUILDER===========*/
		void			buildResponse(); //check statuscode, set header, set body
		void			buildResponseString(); //build to raw string ready to send to client
		
		/*============SETTERS==================*/
		void			setHeader(const std::string &key, const std::string &value);
		void 			setResponseCode(int code);
		void			setBody(const std::string &body);

		/*===========DEBUGGER===================*/
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


#endif