#include <string>
#include <iostream>
#include "Http.hpp"
#include "httpBuffer.hpp"

/// @brief This is where we initially call the recvHttpRequest. I wanted this to be the 
///function called inside the poll loop but this is not allowed. We can change it around later.\n\n
///This function prints the error message based on the result of recvHttpRequest
/// @param httpObj 
/// @param sockFd 
/// @return Void function, prints to std::cerr
void handleHttpRequest(Http &httpObj, int sockFd)
{
	(void)httpObj;
	(void)sockFd;
	// httpBuffer		bufferObj;

	// switch (recvHttpRequest(bufferObj, sockFd))
	// {
	// 	case COMPLETE:
	// 		httpObj.parseRequest(bufferObj.getRecvStr());
	// 		break;
	// 	case INCOMPLETE:
	// 		std::cerr << "Receive buffer error: Incomplete\n";
	// 		break;
	// 	case RECV_ERROR:
	// 		std::cerr << "Receive buffer error\n";
	// 		break;
	// 	case SOCKET_CLOSED:
	// 		std::cerr << "Socket closed\n";
	// 		break;
	// 	case MAXBYTESRECEIVED:
	// 		std::cerr << "Receive error: maxbytes received\n";
	// 		break;
	// 	case TIMEOUT:
	// 		std::cerr << "Timeout error\n";
	// 		break;
	// }
}

std::string handleHttpResponse(Http &httpObject) //must take socket as param
{
	(void)httpObject;
	// httpObject.setResponse();
	// return(httpObject.getResponseString());
	// send();//send to socket
	return("");
}


