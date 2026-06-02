/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:35 by tutku             #+#    #+#             */
/*   Updated: 2026/06/02 14:12:19 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
steps
socket →
setsockopt →
non-blocking →
bind →
listen →
poll →
accept
*/
int Server::setup(void)
{
	if (this->_createSocket() == ERROR)
	{
		closeSocket();
		return ERROR;
	}
	if (this->_setSocketOptions() == ERROR)
	{
		closeSocket();
		return ERROR;
	}
	if (_setNonBlocking(_fd) == ERROR)
	{
		closeSocket();
		return ERROR;
	}
	if (this->_setAddress() == ERROR)
	{
		closeSocket();
		return ERROR;
	}
	if (this->_bindSocket() == ERROR)
	{
		closeSocket();
		return ERROR;
	}
	if (this->_listenSocket() == ERROR)
	{
		closeSocket();
		return ERROR;
	}
	if (this->_initPollEvent() == ERROR)
	{
		closeSocket();
		return ERROR;
	}
	return SUCCESS;
}

// Add fd to the list of fds that poll() should watch
void Server::_addFdToPoll(int fd)
{
	struct pollfd pollFdServer;

	pollFdServer.fd = fd;
	pollFdServer.events = POLLIN;
	pollFdServer.revents = 0;

	_pollFds.push_back(pollFdServer);
}

/*
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
poll() tells you which fd is ready to do something.
poll() only fills revents inside existing _pollFds

fd      = the fd you want to watch
events  = what you are interested in
revents = what actually happened
if fd < 0 --> revents return 0

return values:
> 0   number of fds with events
0   timeout happened
-1   error, wait forever until something happens

POLLIN There is data to read -> a new client is trying to connect
if TIMEOUT is -1, block until an event occurs
*/
int Server::_initPollEvent()
{
	int pollFdCount;

	_addFdToPoll(_fd); // add listening socket fd
	while (1)
	{
		pollFdCount = poll(_pollFds.data(), _pollFds.size(), 1000);
		//_pollFds.push_back(pollfd_for_clientFd);
		//_clients[clientFd] = Client(clientFd);
		if (pollFdCount == ERROR) //error and wait
		{
			continue;
		}
		for (int i = 0; i < (int)(_pollFds.size()); i++)
		{
			if (_pollFds[i].revents != 0)
			{
				printf("  fd=%d; events: %s%s%s\n", _pollFds[i].fd,
					   (_pollFds[i].revents & POLLIN) ? "POLLIN " : "",
					   (_pollFds[i].revents & POLLHUP) ? "POLLHUP " : "",
					   (_pollFds[i].revents & POLLERR) ? "POLLERR " : "");
				if (_pollFds[i].fd == _fd && _pollFds[i].revents & POLLIN)
				{
					// _accept();
				}
			}
		}
			// handle timeouts
			// check all fds
			if (pollFdCount == 0) // timeout
			{
				continue;
			}
	}
}

/*
in case main is like, default values for host and port
	Server server;
	server.setup();
*/
Server::Server() : _port(8080), _host(INADDR_ANY), _fd(-1)
{
	memset(&_address, 0, sizeof(_address));
}

Server::Server(uint32_t host, int port)
	: _port(port), _host(host), _fd(-1)
{
	memset(&_address, 0, sizeof(_address));
}

Server::~Server()
{
	closeSocket();
}

int Server::get_fd() const
{
	return (this->_fd);
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
