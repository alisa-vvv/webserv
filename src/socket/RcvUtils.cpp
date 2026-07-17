#include "Server.hpp"
#include "Http.hpp"

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
		{
			client.setRecvStatus(status);
			return SERVER_OK;
		}
		if (status == RECV_ERROR)
		{
			//400 err?
			client.setRecvStatus(status);
			return SERVER_OK;
		}

		if (status == COMPLETE)
		{
			std::string response = clientHandler(client.getListenerClass(), bufferObj.getRecvStr());
			client.setResponse(response.c_str());
			client.setRecvStatus(status);
			client.setResponseStatus(true);
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

eServerError Server::_handleSend(Client& client)
{
	int socketFd = client.getClientFd();
	const char *response = client.updatedResponse(client.getResponse());
	std::cout << "GOES TO SEND" << std::endl; 
	
	send(socketFd, response, sizeof(response), 0);
	std::cout << "finisHED SEND" << std::endl; 

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
