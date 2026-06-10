#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include <map>

enum httpType {
	REQUEST,
	RESPONSE
};

enum httpVersion {
	HTTP_1_0,
	HTTP_1_1,
	HTTP_2,
	INVALID
};

std::map<int, std::string> HTTP_STATUS_MESSAGE = {
	{200, "OK"},
	{201, "Created"},
	{400, "Bad request"},
	{401, "Unauthorized"},
	{403, "Forbidden"}, //no permission
	{404, "Not Found"},
	{408, "Request Timeout"},
	{500, "Internal Server Error"}
}; //405 408 409 413 415 500 502 504

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
		int									_contentLen;
		bool								_hasBody;
		bool								_hasExtension;
		std::string							_uri;
		std::string							_body;
		std::map<std::string, std::string>	_headers;

	public:
		Http();
		void			parseRequest(std::string rawString);
		void			parseRequestLine(const std::string line);
		void			parseHeaders(const std::string &headers);
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
		std::string		getResponseString(); //back to raw string for response

	
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


enum receiveStatus{
	SOCKET_CLOSED,
	RECV_ERROR,
	COMPLETE,
	INCOMPLETE,
	MAXBYTESRECEIVED
};

class httpBuffer {
	private:
	std::string				recvStr;
	public:
	httpBuffer();
	static const int		maxRequest = 8192;
	ssize_t					totalBytesReceived;
	receiveStatus			currentBufferStatus;
	receiveStatus			checkStatus();
	void					append(char *buffer, ssize_t size);
	const char*				getRecvStr() const { return this->recvStr.c_str(); }
};

#endif