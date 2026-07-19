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

/*
send(
	socketFd,       // which socket
	dataPointer,    // where the data starts
	dataSize,       // how many bytes to try sending
	flags           // usually 0
);
*/
eServerError Server::_handleSend(Client& client)
{
	int clientFd = client.getClientFd();
	const std::string &response = client.getResponse();
	const char *responseStart = response.c_str() + client.getBytesSent();
	size_t sendSize = response.size() - client.getBytesSent();

	if (client.getBytesSent() > response.size())
		return SERVER_SEND_ERR;
	if (client.getBytesSent() == response.size())
		return SERVER_OK;
	ssize_t result = send(clientFd, responseStart, sendSize, 0);
	if (result == ERROR)
	{
		std::cerr << "send() failed" << std::endl;
		return SERVER_SEND_ERR;
	}
	else if (result > 0)
	{
		client.updateBytesSent(static_cast<size_t>(result));
		client.updateLastActivity();
	}
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
