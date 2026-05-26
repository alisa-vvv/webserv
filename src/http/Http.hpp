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
	HTTP_2
};

std::map<int, std::string> STATUS_MESSAGE = {
	{200, "OK"},
	{201, "Created"},
	{400, "Bad request"},
	{401, "Unauthorized"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{408, "Request Timeout"},
	{500, "Internal Server Error"}
};

enum httpMethod {
	GET,
	POST,
	DELETE,
	UNKNOWN
};

class Http {
	private:
		httpType							type;
		httpMethod							method;
		httpVersion							version;
		int									statusCode;
		int									contentLen;
		bool								hasBody;
		std::string							uri;
		std::string							body;
		std::map<std::string, std::string>	headers;

	public:
		Http();
		void			parseRequest(std::string rawString);
	
		httpType		getType() const;
		httpMethod		getMethod() const;
		int				getContentLen() const;
		std::string		getBody() const;
		std::string		getHeader(const std::string &key) const;
		std::string		getUri() const;
		httpVersion		getVersion() const;
		int				getStatusCode() const;
	
		void			setResponse(int code); //give statuscode, set header, set body
		void			setHeader(const std::string &key, const std::string &value);
		void			setBody(const std::string &body);

		std::string		getResponseString(); //back to raw string for response
		void			debugPrint();
		
	class HttpException : public std::exception {
		private:
			int statusCode;
			std::string message;
		public:
			HttpException(int code) : statusCode(code){
				message = "HTTP Error " + std::to_string(code) + ": " 
				+ STATUS_MESSAGE.at(code);
			}
			const char *what() const noexcept override {
				return message.c_str();
			}
	}
};

#endif