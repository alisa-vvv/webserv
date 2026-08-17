/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollEvent.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 13:50:39 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/08/17 22:30:49 by tutku            ###   ########.fr       */
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
			std::cerr << "poll() failed: " << std::strerror(errno) << std::endl;
			return SERVER_POLL_ERR;
		}
		if (pollFdCount > 0)
		{
			eServerError err;

			err = _pollEvents();
			if (err != SERVER_OK)
				return err;
		}
		_checkClientTimeouts();
		//add CGI timeout stuff
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
		
		printPollInfo(i); // test

		if (_isListenerFd(fd))
		{
			err = _handleListenerEvent(i);
			if (err != SERVER_OK)
				return err;
			i++;
		}
		else if (_isCgiFd(fd)) //TODO:finish
		{
			eClientEventResult clientClosed = _handleCgiEvent(fd, i);
			if (clientClosed == CLIENT_KEPT)
				i++; 
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
		std::cerr << "Listening socket error on fd " << fd << std::endl;
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
			clientState state = _clients.at(fd).getHttpClass().getState();

			if (state == READY_TO_SEND)
				_pollFds[i].events = POLLOUT;
			else if (state == HANDLING_CGI_EXTENSION
				|| state == HANDLING_CGI_STATIC)
			{
				if (_startCgi(fd) != SERVER_OK)
				{
					_closeClientFd(fd);
					return CLIENT_REMOVED;
				}
				_pollFds[i].events = 0;
			}
		}
	}
	if (_pollFds[i].revents & POLLOUT)
	{
		std::cout << "Calling send for client " << fd << std::endl;

		std::cout << "Generated response:\n"
				<< _clients.at(fd).getResponse()
				<< "\n--- response end ---\n";

		eServerError err = _handleSend(_clients.at(fd));
		if (err != SERVER_OK)
		{
			_closeClientFd(fd);
			return CLIENT_REMOVED;
		}
		if (_clients.at(fd).isResponseComplete())
		{
			std::cout << "Response completely sent to client "
					<< fd << std::endl;
			_closeClientFd(fd);
			return CLIENT_REMOVED;
		}
	}
	return CLIENT_KEPT;
}

