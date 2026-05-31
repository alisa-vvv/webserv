/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:35 by tutku             #+#    #+#             */
/*   Updated: 2026/05/31 18:42:38 by tutku            ###   ########.fr       */
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

/*
	Sets a file descriptor to non-blocking mode.

	fd:
		The file descriptor to modify. This can be a listening socket,
		client socket, or another fd such as a pipe.

	How it works:
		1. F_GETFL gets the fd's current file status flags.
		2. O_NONBLOCK is added to those flags.
		3. F_SETFL applies the updated flags back to the fd.

	Return:
		SUCCESS on success, ERROR on failure.
*/
int Server::_setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == ERROR)
	{
		std::cerr << "Error fcntl F_GETFL: " << std::strerror(errno) << std::endl;
		return ERROR;
	}
	int status = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	if (status == ERROR)
	{
		std::cerr << "Error making fd non-blocking: " << std::strerror(errno) << std::endl;
		return ERROR;
	}
	return SUCCESS;
}

/*
	create a socket and make it non-blocking
	AF_INET : IPv4 protocol
	SOCK_STREAM: TCP socket
	0: default protocol
*/
int Server::_createSocket()
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == ERROR)
	{
		std::cerr << "Error opening socket: " << std::strerror(errno) << std::endl;
		return ERROR;
	}
	std::cout << "Socket created successfully!" << std::endl;
	return SUCCESS;
}

/*
 SO_REUSEADDR option for when you restart the server, dont get:
	Address already in use
*/
int Server::_setSocketOptions()
{
	int opt = 1;

	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == ERROR)
	{
		std::cerr << "Error setsockopt: " << std::strerror(errno) << std::endl;
		return ERROR;
	}
	return SUCCESS;
}

/*
takes _host and _port
converts them  into sockaddr_in
Store result in _address
*/
int Server::_setAddress()
{
	memset(&_address, 0, sizeof(_address));

	_address.sin_family = AF_INET;
	_address.sin_port = htons(_port);
	_address.sin_addr.s_addr = htonl(_host);

	return SUCCESS;
}

// finish this function by checking the documentation
// https://www.linuxhowtos.org/C_C++/socket.htm
int Server::_bindSocket()
{
	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) == ERROR)
	{
		std::cerr << "Couldn't bind the port!: " << std::strerror(errno) << std::endl;
		return ERROR;
	}
	std::cout << "Bind successful" << std::endl;
	return SUCCESS;
}

/*
int listen(int sockfd, int backlog);

backlog is the max num of connections
	that will be queued while the server is busy
*/
int Server::_listenSocket()
{
	if (listen(this->_fd, BACKLOG) == ERROR)
	{
		std::cerr << "Couldn't listen socket!: " << std::strerror(errno) << std::endl;
		return ERROR;
	}
	std::cout << "Listen successful" << std::endl;
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
		for (int i = 0; i < _pollFds.size(); i++)
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
Use listening/server fd to accept one waiting connection.
It returns a new client fd.
*/
int Server::_accept(int serverListenFd)
{
	while (1)
	{
		int clientFd = accept(serverListenFd, NULL, NULL);

		if (clientFd >= 0)
		{
			if (_setNonBlocking(clientFd) == ERROR)
			{
				close(clientFd);
				continue;
			}
			_addFdToPoll(clientFd);
			continue;
		}
		else if (errno == EWOULDBLOCK || errno == EAGAIN)
			break;
		// TODO: add if case for signals:
		// check man page for accept() -> EINTR and ECONNABORTED
		std::cerr << "Error accept(): " << std::strerror(errno) << std::endl;
		break;
	}
	return SUCCESS;
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

void Server::closeSocket()
{
	if (_fd != -1)
	{
		close(_fd);
		_fd = -1;
	}
}

//test
void Server::printPortNumber()
{
	 /* Find out assigned port number and print it out. */
	int length = sizeof(_address);
	if (getsockname(_fd, (struct sockaddr *)&_address, (socklen_t*)&length))
	{
		perror("getting socket name");
		exit(1);
	}
	printf("Socket has port #%d\n", ntohs(_address.sin_port));
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
