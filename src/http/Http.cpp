#include <string>
#include <iostream>
#include <sys/socket.h>
#include "Http.hpp"

httpBuffer::httpBuffer()
	:totalBytesReceived(0)
{}

void httpBuffer::append(char *buffer, ssize_t size)
{
	this->recvStr.append(buffer, size);
}

receiveStatus httpBuffer::checkStatus()
{
	const char	*contLenStr = "Content-Length:";
	const char	*contLenPtr;
	int			contLenCnt = 0;
	int			headerEnd = 0;
	int			bodyReceived = 0;

	size_t pos = this->recvStr.find("\r\n\r\n");
	if (pos == std::string::npos)
		return (this->currentBufferStatus = INCOMPLETE);
	contLenPtr = strstr(this->recvStr.c_str(), contLenStr);
	if (!contLenPtr)
	{
		this->currentBufferStatus = COMPLETE;
		return COMPLETE;
	}
	sscanf(contLenPtr, "Content-Length: %d", &contLenCnt);
	headerEnd = pos + strlen("\r\n\r\n");
	bodyReceived = this->recvStr.size() - headerEnd;
	if (bodyReceived >= contLenCnt)
	{
		this->currentBufferStatus = COMPLETE;
		return COMPLETE;
	}
	return (this->currentBufferStatus = INCOMPLETE);
}

receiveStatus recvHttpRequest(httpBuffer &bufferObj, int sockFd) //must take socket as param
{
	ssize_t			bytesRead;
	char			buffer[4096];

	//add client last activity
	while (bufferObj.totalBytesReceived < bufferObj.maxRequest)
	{
		bytesRead = recv(sockFd, buffer, sizeof(buffer), 0);
		if (bytesRead == 0)
			return SOCKET_CLOSED;
		if (bytesRead < 0)
			return RECV_ERROR;
		bufferObj.totalBytesReceived += bytesRead;
		bufferObj.append(buffer, bytesRead);
		bufferObj.checkStatus();
		if (bufferObj.currentBufferStatus == COMPLETE)
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
			httpObj.parseRequest(bufferObj.getRecvStr());
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


