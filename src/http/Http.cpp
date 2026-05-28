#include <string>
#include <iostream>
#include <sys/socket.h>
#include "Http.hpp"

//Add recvHTTP request to main
//add a

enum receiveStatus{
	SOCKET_CLOSED,
	RECV_ERROR,
	COMPLETE,
	INCOMPLETE,
	MAXBYTESRECEIVED
};

class httpBuffer {
	public:
	httpBuffer();
	static const int		maxRequest = 8192;
	ssize_t					totalBytesReceived;
	receiveStatus			curentBufferStatus;
	bool					isBufferComplete;
	char					bufferLine[maxRequest];
	receiveStatus			checkStatus(char *buffer);
	void					reset();
	void					append();
};

receiveStatus checkStatus(char *buffer)
{
	
}

httpBuffer::httpBuffer()
	:totalBytesReceived(0), isBufferComplete(false)
	{};


receiveStatus recvHttpRequest(httpBuffer bufferObj, int sockFd) //must take socket as param
{
	ssize_t			bytesRead;

	//add client last activity?
	while (bufferObj.totalBytesReceived < bufferObj.maxRequest)
	{
		bytesRead = recv(sockFd, bufferObj.bufferLine + bufferObj.totalBytesReceived, bufferObj.maxRequest - bufferObj.totalBytesReceived, 0);
		if (bytesRead == 0)
			return SOCKET_CLOSED;
		if (bytesRead < 0)
			return RECV_ERROR;
		bufferObj.totalBytesReceived += bytesRead;
		bufferObj.checkStatus(bufferObj.bufferLine);
		if (bufferObj.curentBufferStatus == COMPLETE)
			return COMPLETE;
	}
	return MAXBYTESRECEIVED;
}
	

void handleHttpRequest(Http &httpObj, int sockFd)
{
	httpBuffer		bufferObj;

	switch (recvHttpRequest(bufferObj, sockFd))
	{
		case COMPLETE:
			httpObj.parseRequest(bufferObj.bufferLine);
			break;
		case INCOMPLETE:
			std::cerr << "Receive buffer error: Incomplete\n";
			break;
		case RECV_ERROR:
			std::cerr << "Receive buffer error\n";
			break;
		case SOCKET_CLOSED:
			std::cerr << "Socket closed\n";
			break;
		case MAXBYTESRECEIVED:
			std::cerr << "Receive error: maxbytes received\n";
			break;
	}
}

std::string handleHttpResponse(Http &httpObject) //must take socket as param
{
	httpObject.setResponse();
	return(httpObject.getResponseString());
	send();//send to socket
}


