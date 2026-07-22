#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include <map>
#include <filesystem>
#include <tuple>
#include <fstream>
#include "Client.hpp"
#include "Colors.hpp"
#include "configParser.hpp"
#include "HttpError.hpp"
#include "Listener.hpp"

#include "Timer.hpp"
#include <optional>


//success defined in listener
static const int FAILURE = -1;
static const int NOT_VAR = 0;
static const int IS_VAR = 1;

enum httpVersion {
	HTTP_1_0,
	HTTP_1_1,
	INVALID //we will not handle http versions other than 1.0 and 1.1, so we set INVALID for unsupported versions
};

/// @brief This is a struct with references to the correct config file to use. context struct
struct requestConfig
{
	const cfg_server_t	*server;
	const t_location	*location;
};

class Http {
	private:
		clientState							_state;
		httpMethod							_method;
		httpVersion							_version;
		int									_statusCode;
		int									_contentLen;
		bool								_hasBody;
		bool								_hasExtension;
		size_t								_clientMaxBodySize;
		std::string							_receivedUri; //uri from original request
		std::string							_builtUri; //uri built with root etc
		std::string							_body;
		std::string							_responseString;
		std::multimap<std::string, std::string>	_requestHeaders;
		std::multimap<std::string, std::string>	_responseHeaders;

	public:
		Http();
		requestConfig	requestConfig;

		/*==========PARSING===========*/
		void			parseRequest(const std::string &rawString);
		void			parseRequestLine(const std::string line);
		void			parseHeaders(const std::string &headers);

		/*==========VALIDATION========*/
		int				validateURI(std::string uri);
		void			validateLayer();
		void			validateFile();
		void			buildAbsoluteUri();
	std::string resolveUri(const std::string &uri);

		/*==========ERROR LOADER================*/
		void			handleErrorResponse(); //load error page content into body based on status
		
		/*===========RESPONSE BUILDER===========*/
		void			buildResponse(); //check statuscode, set header, set body
		std::string		buildCGIResponseString(std::string cgiResponse);
		void			buildResponseString(); //build to raw string ready to send to client

		/*==========METHOD HANDLERS==========*/
		void 			handleGetResponse();
		void 			handlePostResponse();
		void 			handleDeleteResponse();
		void			handleAutoIndexResponse();
		void			handleReturnResponse();

		/*==========GETTERS============*/
		clientState		getState() const;
		httpMethod		getMethod() const;
		int				getContentLen() const;
		std::string		getBody() const;
		std::string		getHeader(const std::string &key) const;
		httpVersion		getVersion() const;
		int				getStatusCode() const;
		std::string		getResponseString() const;
		bool			getExtension() const;
		std::string		getBuiltUri() const;
		std::string		getReceivedUri() const;
		std::string		getContentTypeExtension(const std::string &contentType) const;
		std::uintmax_t	getClientMaxBodySize();


		/*============SETTERS==================*/
		void			setRequestHeader(const std::string &key, const std::string &value);
		void			setResponseHeader(const std::string &key, const std::string &value);
		void 			setResponseCode(int code);
		void			setExtension(bool status);
		void			setBody(); //built from uri
		void			setRawBody(const std::string &body); //direct assignment
		void			setBody(const std::string uri); //assign from filepath
		void			setState(clientState state);
		void			setContentType();
		void			setClientMaxBodySize();
		
		/*=======REQUEST Config===================*/
		int				findRequestConfig(const Listener *listener);
		void 			setRequestConfig(const Listener *listener);



		/*===========DEBUGGER===================*/
		void			debugPrintRequest();
		void			debugPrintRequestConfig();
		void			debugPrintHttpClassAttributes();
		void			printError(std::string error, std::string functName, bool isVar);
};

void	clientHandler(Client &client, Http &http, std::string recvStr);

typedef struct	cgi_t {
	int							child_pid = -1;
	int							input = -1;
	int							output = -1;
	std::string					input_string = "";
	std::string					output_string = "";
	time_point<system_clock>	timer;
	Http						request_data;
}	cgi_t;

#endif
