/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 14:04:20 by tutku             #+#    #+#             */
/*   Updated: 2026/06/04 11:44:40 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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

			Client newClient(clientFd);
			_clients[clientFd] = newClient;
			
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

void Server::closeSocket()
{
	if (_fd != -1)
	{
		close(_fd);
		_fd = -1;
	}
}

// test
void Server::printPortNumber()
{
	/* Find out assigned port number and print it out. */
	int length = sizeof(_address);
	if (getsockname(_fd, (struct sockaddr *)&_address, (socklen_t *)&length))
	{
		perror("getting socket name");
		exit(1);
	}
	printf("Socket has port #%d\n", ntohs(_address.sin_port));
}

int Server::_printSocketName()
{
	struct sockaddr_in addr;
	socklen_t len;

	memset(&addr, 0, sizeof(addr));
	len = sizeof(addr);

	if (getsockname(_fd, (struct sockaddr *)&addr, &len) == ERROR)
	{
		std::cerr << "getsockname failed" << std::endl;
		return ERROR;
	}

	std::cout << "Bound port: " << ntohs(addr.sin_port) << std::endl;

	return SUCCESS;
}
