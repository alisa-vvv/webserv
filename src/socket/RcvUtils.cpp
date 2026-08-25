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

		bufferObj.totalBytesReceived += bytesRead;
		bufferObj.append(buffer, bytesRead);
		
		const std::string infoMsg = "bytes=" + std::to_string(bytesRead);
		_printDebug("[RECV]", client, infoMsg, false);

		const std::string &requestStr = bufferObj.getRecvStr();
		if (requestStr.size() > MAX_REQUEST)
		{
			client.setRecvStatus(RECV_ERROR);
			_printDebug("[RECV ERROR]", client, "request too large", true);
			return SERVER_RECV_ERR;
		}
		return (_checkRecvBuffer(client));
	}
	else if (bytesRead == 0)
	{
		_printDebug("[DISCONNECT]", client, "client closed connection", false);

		return SERVER_CLIENT_CLOSED;
	}

	_printDebug("[RECV ERROR]", client, "recv failed", true);
	return SERVER_RECV_ERR;
}

eServerError	Server::_checkRecvBuffer(Client &client)
{
	RcvBuffer &bufferObj = client.getRcvBuffer();
	receiveStatus status = bufferObj.checkStatus();

	client.setRecvStatus(status);
	if (status == INCOMPLETE)
		return SERVER_OK;
	if (status == RECV_ERROR)
	{
		_printDebug("[RECV REQUEST ERROR]", client, "invalid request", true);
		return SERVER_RECV_ERR;
	}
	if (status == COMPLETE)
	{
		client.clientHandler(bufferObj.getRecvStr());
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
	{
		const std::string infoMsg = "sent=" + std::to_string(client.getBytesSent()) + " response=" + std::to_string(response.size());
		_printDebug("[SEND ERROR]", client, infoMsg, true);
		return SERVER_SEND_ERR;
	}
	if (client.isResponseComplete())
	{
		client.setResponseStatus(false);
		return SERVER_OK;
	}

	const char *responseStart = response.c_str() + client.getBytesSent();
	size_t remaining = response.size() - client.getBytesSent();

	ssize_t result = send(clientFd, responseStart, remaining, 0);

	if (result == ERROR)
	{
		_printDebug("[SEND ERROR]", client, "send failed", true);

		return SERVER_SEND_ERR;
	}
	else if (result == 0)
	{
		_printDebug("[SEND ERROR]", client, "send returned 0", true);
		return SERVER_SEND_ERR;
	}
	else if (result > 0)
	{
		const std::string infoMsg = "bytes=" + std::to_string(result) + " total=" + std::to_string(client.getBytesSent()) + "/" + std::to_string(response.size());
		_printDebug("[SEND]", client, infoMsg, false);

		client.updateBytesSent(static_cast<size_t>(result));
		client.updateLastActivity();

		if (client.isResponseComplete())
		{
			client.setResponseStatus(false);

			const std::string completeMsg = "bytes=" + std::to_string(client.getBytesSent());
			_printDebug("[SEND COMPLETE]", client, completeMsg, false);
			std::cout << std::endl;
		}
	}
	return SERVER_OK;
}
