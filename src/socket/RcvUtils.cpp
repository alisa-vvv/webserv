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
	(void)clientFd; //TODO:remove
	return SERVER_OK;
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
