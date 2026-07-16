#include "Server.hpp"

eServerError Server::_handleRecv(int clientFd)
{
	Client &client = _clients.at(clientFd);
	RcvBuffer &bufferObj = client.getRcvBuffer();

	char buffer[BUFFER_MAX];
	ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);

	if (bytesRead > 0)
	{
		client.updateLastActivity();
		std::cout << "Received "
					<< bytesRead
					<< " bytes from client" << std::endl; // test

		bufferObj.totalBytesReceived += bytesRead;
		// TODO: check for total bytes?

		bufferObj.append(buffer, bytesRead);
		receiveStatus status = bufferObj.checkStatus();
		if (status == INCOMPLETE)
			return SERVER_OK;
		if (status == RECV_ERROR)
		{
			//400 err?
			return SERVER_OK;
		}

		if (status == COMPLETE)
		{
			// call http parser here
		}
		return SERVER_OK;
	}
	else if (bytesRead == 0) // the remote side has closed the connection on you
	{
		std::cout << "Client " << clientFd
					<< " closed the connection" << std::endl;
		return SERVER_CLIENT_CLOSED;
	}
	std::cerr << "recv() failed for client " << clientFd
				<< ": " << std::strerror(errno) << std::endl;
	// TODO: decide how to consider
	return SERVER_RECV_ERR;
}

eServerError Server::_handleSend(int clientFd)
{

}

receiveStatus RcvBuffer::checkStatus()
{
	int contLenCnt = -1;
	bool hasContentLen = false;

	size_t headerEnd = this->recvStr.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
	{
		// if (recvStr.size() > ) // add define MAX HEADER SIZE
		// 	return RECV_ERROR;
		return (INCOMPLETE);
	}

	std::string headers = this->recvStr.substr(0, headerEnd);
	size_t contentLenPos = headers.find("Content-Length:");

	size_t bodyStart = headerEnd + 4;
	size_t bodyReceived = this->recvStr.size() - bodyStart;

	if (contentLenPos != std::string::npos)
	{
		hasContentLen = true;
		int result = sscanf(headers.c_str() + contentLenPos, "Content-Length: %d", &contLenCnt);

		if (result != 1 || contLenCnt < 0)
			return RECV_ERROR;
	}
	if (!hasContentLen)
	{
		// if (bodyReceived > 0)
		// 	return RECV_ERROR;
		return COMPLETE;
	}
	if (bodyReceived == contLenCnt)
		return COMPLETE;

	if (bodyReceived < contLenCnt)
	return (INCOMPLETE);
	
	return RECV_ERROR;
}

//recv with the while loop
// eServerError Server::_handleRecv(int clientFd)
// {
	
// 	Client		&client = _clients.at(clientFd);
// 	RcvBuffer	&bufferObj = client.getRcvBuffer();

// 	char		buffer[BUFFER_MAX];	
// 	ssize_t		bytesRead;

// 	while (1)
// 	{
// 		bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);
// 		if (bytesRead > 0)
// 		{
// 			client.updateLastActivity();
// 			std::cout << "Received "
// 					  << bytesRead 
// 					  << " bytes from client" << std::endl; //test

// 			bufferObj.totalBytesReceived += bytesRead;
// 			//TODO: check for total bytes

// 			bufferObj.append(buffer, bytesRead);
// 			//check status
// 		}
// 		else if (bytesRead == 0) //the remote side has closed the connection on you
// 		{
// 			std::cout << "Client " << clientFd
// 					  << " closed the connection" << std::endl;
// 			return SERVER_CLIENT_CLOSED;
// 		}
// 		else
// 		{
// 			std::cerr << "recv() failed for client " << clientFd
// 					  << ": " << std::strerror(errno) << std::endl;
// 			//TODO: decide how to consider
// 			return SERVER_RECV_ERR;
// 		}
// 	}
// 	return SERVER_OK;
// }
