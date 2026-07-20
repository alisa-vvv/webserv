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
			return SERVER_OK;
		if (status == RECV_ERROR)
		{
			//400 err?
			return SERVER_OK;
		}

		if (status == COMPLETE)
		{
			Http clientHttp;
			// if (state == READY_TO_SEND)
				//call this and that
			//call Http class here, if 
			std::cout << "DEBUG enters status complete " << std::endl;

			std::string response = clientHandler(client.getListenerClass(), bufferObj.getRecvStr());
			// if (state == HANDLING_CGI_EXTENSION){
				
			// }
			//
			std::cout << "DEBUG response string: " << response << std::endl;
			std::cout << "fuck yeah " << std::endl;
			//if(state == READY_TO_SEND)
				_handleSend(client, response);
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

eServerError Server::_handleSend(Client client, std::string response)
{
	//int socketFd = client.getListenerFd();

	//int len = response.length();
	//const char *responseChar = response.c_str();

	//strcpy(responseChar, response.c_str());

	//send(socketFd, responseChar, response.size(), MSG_OOB);
	(void) response;
	(void)client;
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
