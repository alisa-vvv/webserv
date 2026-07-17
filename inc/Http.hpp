#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include <map>
#include <filesystem>
#include <tuple>
#include <fstream>
#include "Listener.hpp"
#include "configParser.hpp"
#include "HttpError.hpp"

//success defined in listener
static const int FAILURE = -1;

enum clientState {
	RECEIVING,
	PARSING,
	VALIDATING,
	HANDLING_CGI_EXTENSION,
	HANDLING_CGI_STATIC,
	READY_TO_SEND,
	CLIENT_ERROR,
};

enum httpVersion {
	HTTP_1_0,
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

		/*==========ERROR LOADER================*/
		void			handleErrorResponse(); //load error page content into body based on status
		
		/*===========RESPONSE BUILDER===========*/
		void			buildResponse(); //check statuscode, set header, set body
		void			buildResponseString(); //build to raw string ready to send to client

		/*==========METHOD HANDLERS==========*/
		void 			handleGetResponse();
		void 			handlePostResponse();
		void 			handleDeleteResponse();
		void			handleAutoIndexResponse();
		void			handleReturnResponse();

		/*============SETTERS==================*/
		void			setRequestHeader(const std::string &key, const std::string &value);
		void			setResponseHeader(const std::string &key, const std::string &value);
		void 			setResponseCode(int code);
		void			setExtension(bool status);
		void			setBody();
		void			setBody(const std::string uri); //overload
		void			setState(clientState state);
		void			setContentType();
		
		/*=======REQUEST Config===================*/
		int				findRequestConfig(const Listener *listener);
		void 			setRequestConfig(const Listener *listener);

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

		/*===========DEBUGGER===================*/
		void			debugPrintRequest();
		void			debugPrintRequestConfig();
};

std::string	clientHandler(const Listener *listener, std::string recvStr);

#endif
