/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 14:04:20 by tutku             #+#    #+#             */
/*   Updated: 2026/06/10 22:55:07 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
	create a socket and make it non-blocking
	AF_INET : IPv4 protocol
	SOCK_STREAM: TCP socket
	0: default protocol
*/
eServerError Server::_createSocket()
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == ERROR)
	{
		std::cerr << "Error opening socket: " << std::strerror(errno) << std::endl;
		return SERVER_CREATESOCK_ERR;
	}
	std::cout << "Socket created successfully!" << std::endl;
	return SERVER_OK;
}

/*
 SO_REUSEADDR option for when you restart the server, dont get:
	Address already in use
*/
eServerError Server::_setSocketOptions()
{
	int opt = 1;

	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == ERROR)
	{
		std::cerr << "Error setsockopt: " << std::strerror(errno) << std::endl;
		return SERVER_SETSOCKOPT_ERR;
	}
	return SERVER_OK;
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
eServerError Server::_setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == ERROR)
	{
		std::cerr << "Error fcntl F_GETFL: " << std::strerror(errno) << std::endl;
		return SERVER_SETNONBLOCKING_ERR;
	}
	int status = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	if (status == ERROR)
	{
		std::cerr << "Error making fd non-blocking: " << std::strerror(errno) << std::endl;
		return SERVER_SETNONBLOCKING_ERR;
	}
	return SERVER_OK;
}

/*
takes _host and _port
converts them  into sockaddr_in
Store result in _address
*/
eServerError Server::_setAddress()
{
	memset(&_address, 0, sizeof(_address));

	_address.sin_family = AF_INET;
	_address.sin_port = htons(_port);
	_address.sin_addr.s_addr = htonl(_host);
	return SERVER_OK;
}

// finish this function by checking the documentation
// https://www.linuxhowtos.org/C_C++/socket.htm
// reinterpret casting-> Keep the same memory address, 
//    but treat the pointer as a different pointer type.
eServerError Server::_bindSocket()
{
	if (bind(_fd, reinterpret_cast<const struct sockaddr *>(&_address), sizeof(_address)) == ERROR)
	{
		std::cerr << "Couldn't bind the port!: " << std::strerror(errno) << std::endl;
		return SERVER_BIND_ERR;
	}
	std::cout << "Bind successful" << std::endl;
	return SERVER_OK;
}

/*
int listen(int sockfd, int backlog);

backlog is the max num of connections
	that will be queued while the server is busy
*/
eServerError Server::_listenSocket()
{
	if (listen(this->_fd, BACKLOG) == ERROR)
	{
		std::cerr << "Couldn't listen socket!: " << std::strerror(errno) << std::endl;
		return SERVER_LISTEN_ERR;
	}
	std::cout << "Listen successful" << std::endl;
	return SERVER_OK;
}

/*
Use listening/server fd to accept one waiting connection.
It returns a new client fd.
*/
eServerError Server::_accept(int serverListenFd)
{
	while (1)
	{
		int clientFd = accept(serverListenFd, NULL, NULL);

		if (clientFd >= 0)
		{
			if (_setNonBlocking(clientFd) != SERVER_OK)
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
		if (errno == EINTR || errno == ECONNABORTED) // check accept() man
			continue;
		std::cerr << "Error accept(): " << std::strerror(errno) << std::endl;
		return SERVER_ACCEPT_ERR;
	}
	return SERVER_OK;
}

void Server::closeSocket()
{
	if (_fd != -1)
	{
		close(_fd);
		_fd = -1;
	}
}
