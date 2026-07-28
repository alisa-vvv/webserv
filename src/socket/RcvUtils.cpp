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
					<< " bytes from client "
					<< clientFd
					<< std::endl;

		bufferObj.totalBytesReceived += bytesRead;
		
		bufferObj.append(buffer, bytesRead);

		const std::string &requestStr = bufferObj.getRecvStr();
		if (requestStr.size() > MAX_REQUEST) //request too large
		{
			client.setRecvStatus(RECV_ERROR);
			return SERVER_RECV_ERR;
		}
		return (_checkRecvBuffer(client));
	}
	else if (bytesRead == 0) // the remote side has closed the connection on you
	{
		std::cout << "Client "
				 << clientFd
				 << " closed the connection"
				 << std::endl;

		return SERVER_CLIENT_CLOSED;
	}
	std::cerr << "recv() failed for client "
			 << clientFd
			 << ": "
			 << std::strerror(errno)
			 << std::endl;
	return SERVER_RECV_ERR;
}

eServerError	Server::_checkRecvBuffer(Client &client)
{
	RcvBuffer &bufferObj = client.getRcvBuffer();
	receiveStatus status = bufferObj.checkStatus();

	if (status == INCOMPLETE)
	{
		client.setRecvStatus(status);
		return SERVER_OK;
	}
	if (status == RECV_ERROR)
	{
		client.setRecvStatus(status);
		return SERVER_RECV_ERR;
	}
	if (status == COMPLETE)
	{
		client.clientHandler(bufferObj.getRecvStr());
		std::cout << "Client response now contains "
			<< client.getResponse().size()
			<< " bytes" << std::endl;
	}
	return SERVER_OK;
}

/*
send(
	socketFd,       // which socket
	dataPointer,    // where the data starts
	dataSize,       // how many bytes to try sending
	flags
);
_responseStatus == true  -> there is a response waiting to be sent
_responseStatus == false -> no response is currently waiting
*/
eServerError Server::_handleSend(Client& client)
{
	int clientFd = client.getClientFd();
	const std::string &response = client.getResponse();

	if (client.getBytesSent() > response.size())
		return SERVER_SEND_ERR;
	if (client.isResponseComplete())
	{
		client.setResponseStatus(false);
		return SERVER_OK;
	}

	const char *responseStart = response.c_str() + client.getBytesSent();
	size_t remaining = response.size() - client.getBytesSent();

	std::cout << "Trying to send " << client.getBytesSent()
			  << " bytes to client " << clientFd << std::endl;
	ssize_t result = send(clientFd, responseStart, remaining, 0);

	if (result == ERROR)
	{
		std::cerr << "send() failed: "
				  << std::strerror(errno) << std::endl;
		return SERVER_SEND_ERR;
	}
	else if (result == 0)
	{
		std::cerr << "send() returned 0 for client "
				  << clientFd << std::endl;
		return SERVER_SEND_ERR;
	}
	else if (result > 0)
	{
		std::cout << "send returned: " << result
				  << ", bytes before: " << client.getBytesSent()
				  << std::endl;

		client.updateBytesSent(static_cast<size_t>(result));
		client.updateLastActivity();

		std::cout << "Sent\nbytes after: " << client.getBytesSent()
				  << ", response size: " << response.size()
				  << std::endl;

		if (client.isResponseComplete())
			client.setResponseStatus(false);
	}
	std::cout << "finisHED SEND" << std::endl; //test

	return SERVER_OK;
}
