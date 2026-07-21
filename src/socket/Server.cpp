/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:35 by tutku             #+#    #+#             */
/*   Updated: 2026/07/21 10:16:21 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
build Listener objs
setup every listening socket
every Listener has its own listening fd
*/
eServerError Server::setup(void)
{
	eListenerError errListenerSetup;

	_buildListener();

	for (size_t i = 0; i < _listeners.size(); i++)
	{
		errListenerSetup = _listeners[i].setup();
		if (errListenerSetup != LISTENER_OK)
		{
			closeListeners();
			return SERVER_LISTENER_SETUP_ERR;
		}
	}
	return SERVER_OK;
}

/*
add every listening socket fd to _pollFds


*/
eServerError Server::run()
{
	eServerError err;

	_addListenerFdsToPoll();
	if (_pollFds.empty()) //server has nothing to listen on
		return SERVER_POLL_ERR;

	err = _initPollEvent();
	if (err != SERVER_OK)
	{
		closeListeners();
		return err;
	}
	_closeAll();
	return SERVER_OK;
}

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

void Server::_checkClientTimeouts()
{
	std::map<int, Client>::iterator it = _clients.begin();

	while (it != _clients.end())
	{
		const int clientFd = it->first;
		const time_point<system_clock> lastActivity = it->second.getLastActivity();
		
		it++;
		if (checkTimeOut(lastActivity, DEFAULT_TIMEOUT_S))
		{
			std::cout << "Client with fd "
						<< clientFd
						<< " timed out"
						<< std::endl;
			_closeClientFd(clientFd);
		}
	}
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
		if (_clients.at(fd).getResponseStatus())
		{
			_pollFds[i].events = POLLOUT;
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

Server::Server(const Config &config) : _config(config)
{
}

Server::~Server()
{
}

// Server &Server::operator= (const Server &other)
// {
// 	if (this != &other)
// 	{
// 		this->_fd = other._fd;
// 		this->_address = other._address;
// 	}
// 	return (*this);
// }

// Server::Server(const Server &other)
// {
// 	*this = other;
// }
