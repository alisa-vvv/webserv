#include "Http.hpp"
#include "httpBuffer.hpp"
#include <sys/socket.h>


/// @brief This is where we initially call the recvHttpRequest. I wanted this to be the 
///function called inside the poll loop but this is not allowed. We need to change it.\n\n
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

//We gotta add the recv to the socket loop, we can just take the while loop?
//the httpBuffer class is in httpBuffer.hpp, the main adding to the 

/// @brief This is where the recv function lives. We need to refactor because the recv needs to be in the main poll loop.
/// @param bufferObj 
/// @param sockFd 
/// @return COMPLETE, INCOMPLETE, TIMEOUT, SOCKET_CLOSED, RECV_ERRIR
receiveStatus recvHttpRequest(httpBuffer &bufferObj, int sockFd) //must take socket as param
{
	ssize_t			bytesRead;
	char			buffer[4096];	
	static const int		TIMEOUTCONST = 30;
	time_t					lastActivity = time(NULL);

	while (1)
	{
		//set http.setState(RECEIVING);
		bytesRead = recv(sockFd, buffer, sizeof(buffer), 0); //recv the string, add to buffer.
		//the buffer gets overwritten every call
		if (bytesRead == 0) // if 0, closed
			return SOCKET_CLOSED;
		if (bytesRead < 0) // if -1 recv errir
			return RECV_ERROR;
		lastActivity = time(NULL); //change last activity to now

		bufferObj.totalBytesReceived += bytesRead; //add recv bytes read to total bytes read
		bufferObj.append(buffer, bytesRead); //append the buffer to the string inside the bufferObj
		if (bufferObj.checkStatus()== COMPLETE) // //check if the request is complete with headers. 
			return COMPLETE;
		if (time(NULL) - lastActivity > TIMEOUTCONST) //timeout check
			return TIMEOUT;
		//if incomplete, keep recving. but this is probably what we need to chnge -> recv must be outside the loop
	}
	return MAXBYTESRECEIVED;
}

