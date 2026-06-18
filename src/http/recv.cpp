#include "Http.hpp"
#include "httpBuffer.hpp"
#include <sys/socket.h>

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
		//if incomplete, keep recving
	}
	return MAXBYTESRECEIVED;
}

//this goes into Http.cpp/handleHttpRequest but we can change it 