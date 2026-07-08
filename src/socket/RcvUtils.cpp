#include "Server.hpp"


eServerError Server::_handleRecv(int clientFd)
{
	
	ssize_t					bytesRead;
	char					buffer[BUFFER_MAX];	
	//time_t					lastActivity = time(NULL);
	int listenerFd = _clients.at(clientFd).getListenerFd();
	bool					isBlocking = false;
	RcvBuffer bufferObj;

	bytesRead = recv(listenerFd, buffer, sizeof(buffer), 0); //recv the string, add to buffer.
	if (bytesRead == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			isBlocking = true;
			return SERVER_OK;
			//todo: timeout check, return SERVER_TIMEOUT_ERR
		}
		else
			return SERVER_RECV_ERR;
	}
	bufferObj = _clients.at(clientFd).getRcvBuffer();
	bufferObj.totalBytesReceived += bytesRead; //add recv bytes read to total bytes read
	bufferObj.append(buffer, bytesRead); //append the buffer to the string inside the bufferObj
	if (bufferObj.checkStatus()== COMPLETE) // //check if the request is complete with headers. 
			return SERVER_OK;
	if (isBlocking)
	{
		//sleep?
	}
	return SERVER_OK;
}


//POST 



///conc

//erts/stars.py HTTP/1.1
//Host: api.example.com
//Content-Type: application/json
//Content-Length: 37
//Connection: keep-alive
//Authorization: Bearer eyJhbGci...

///r/n/r/n ->blank line indicates separator for header and body

//{"username": "ally", "active": true}