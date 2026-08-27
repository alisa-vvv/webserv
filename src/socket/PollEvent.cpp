/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollEvent.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 13:50:39 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/08/27 09:56:44 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
Wait for something to happen on any fd in _pollFds.
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
poll() tells you which fd is ready to do something.
poll() only fills revents inside existing _pollFds

This function does not normally return until:
	- Ctrl-C (gStop = 1)/ SIGINT
	- SIGTERM
	- poll/listener error

_pollFds contains:
	- all listener FDs
	- all currently connected client FDs

POLLIN There is data to read -> a new client is trying to connect
POLLHUP means the socket is no longer connected

listener FDs → new connections
client FDs   → incoming requests or writable sockets
*/
eServerError Server::_initPollEvent()
{
	int pollFdCount;

	printSection("SERVER RUNS SUCCESSFULLY");
	while (gStop == 0)
	{
		pollFdCount = poll(_pollFds.data(), _pollFds.size(), POLL_TIMEOUT_MS);

		if (pollFdCount == ERROR)
		{
			if (errno == EINTR) // if a signal interrupts poll
			{
				if (gStop == 1)
					break;
				continue; // poll should start again
			}

			const std::string infoMsg = "failed: " + std::string(std::strerror(errno));
			printDebug("[POLL ERROR]", infoMsg, true);

			return SERVER_POLL_ERR;
		}
		if (pollFdCount > 0)
		{
			eServerError err;

			err = _pollEvents();
			if (err != SERVER_OK)
				return err;
		}
		_checkTimeouts();
	}
	return SERVER_OK;
}

eServerError Server::_pollEvents()
{
	eServerError err;
	size_t i = 0;

	while (i < _pollFds.size())
	{
		int fd = _pollFds[i].fd;

		if (_pollFds[i].revents == 0)
		{
			i++;
			continue;
		}

		if (_isListenerFd(fd))
		{
			err = _handleListenerEvent(i);
			if (err != SERVER_OK)
				return err;
			i++;
		}
		else if (_isCgiFd(fd))
		{
			eClientEventResult clientClosed = handleCgiEvent(fd, i);
			if (clientClosed == CLIENT_KEPT)
				i++; 
		}
		else
		{
			eClientEventResult clientClosed = _handleClientEvent(i);
			if (clientClosed == CLIENT_KEPT)
				i++;
		}
	}
	return SERVER_OK;
}


// this listenerfd can accept new connections
eServerError Server::_handleListenerEvent(int i)
{
	eServerError err;
	int fd = _pollFds[i].fd;

	if (_pollFds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
	{
		const std::string infoMsg = "POLLHUP | POLLERR | POLLNVAL";
		printDebug("[LISTENER ERROR]", fd, infoMsg, true);

		return SERVER_POLL_ERR;
	}

	if (_pollFds[i].revents & POLLIN)
	{
		err = _acceptClients(fd);
		if (err != SERVER_OK)
			return err;
	}
	return SERVER_OK;
}

eClientEventResult Server::_handleClientEvent(int i)
{
	int fd = _pollFds[i].fd;

	if (_pollFds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
	{
		_closeClientFd(fd);
		return CLIENT_REMOVED;
	}
	if (_pollFds[i].revents & POLLIN)
	{
		eServerError err = _handleRecv(fd);
		if (err != SERVER_OK)
		{
			_closeClientFd(fd);
			return CLIENT_REMOVED;
		}
		if (_clients.at(fd).getRecvStatus() == COMPLETE)
		{
			const std::string infoMsg = "bytes=" + std::to_string(_clients.at(fd).getRcvBuffer().totalBytesReceived);
			printDebug("[RECV COMPLETE]", _clients.at(fd), infoMsg, false);
			
			clientState state = _clients.at(fd).getHttpClass().getState();

			if (state == READY_TO_SEND)
			{
				_pollFds[i].events = POLLOUT;
				return CLIENT_KEPT;
			}
			else if (state == HANDLING_CGI_EXTENSION)
			{
				if (_startCgi(fd) != SERVER_OK)
				{
					_closeClientFd(fd);
					return CLIENT_REMOVED;
				}
				_pollFds[i].events = 0;
				return CLIENT_KEPT;
			}
		}
	}
	if (_pollFds[i].revents & POLLOUT)
	{
		eServerError err = _handleSend(_clients.at(fd));
		if (err != SERVER_OK)
		{
			_closeClientFd(fd);
			return CLIENT_REMOVED;
		}
		if (_clients.at(fd).isResponseComplete())
		{
			_closeClientFd(fd);
			return CLIENT_REMOVED;
		}
	}
	return CLIENT_KEPT;
}

eClientEventResult Server::handleCgiEvent(int cgiFd, int i)
{
	int clientFd = _cgiFdToClientFd.at(cgiFd);

	if (_pollFds[i].revents & (POLLERR | POLLNVAL))
	{
		std::string infoMsg;

		if (_pollFds[i].revents & POLLERR)
			infoMsg += "POLLERR ";
		if (_pollFds[i].revents & POLLNVAL)
			infoMsg += "POLLNVAL";
		printDebug("[CGI POLL ERROR]", _clients.at(clientFd), cgiFd, infoMsg, true);

		removeActiveCgi(cgiFd);
		_closeClientFd(clientFd);
		return CLIENT_REMOVED;
	}

	int isCgiDone = checkCgiDone(_backgroundCgis.at(cgiFd));
	
	if (isCgiDone == -1)
	{
		printDebug("[CGI ERROR]", _clients.at(clientFd), cgiFd, "execution failed", true);

		removeActiveCgi(cgiFd);
		_sendTimeoutResponse(_clients.at(clientFd), HTTP_GATEWAY_TIMEOUT);
		return CLIENT_REMOVED;
	}
	if (isCgiDone == 0) // cgi still running
	{
		return CLIENT_KEPT;
	}

	// cgi finished and response ready
	printDebug("[CGI DONE]", _clients.at(clientFd), cgiFd, "", false);
	copyCgiResponse(cgiFd, clientFd);

	_setPollout(clientFd);
	return CLIENT_REMOVED;
}

void Server::_setPollout(int clientFd)
{
	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].fd == clientFd)
		{
			_pollFds[i].events = POLLOUT;
			return;
		}
	}
}
