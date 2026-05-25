/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 21:31:15 by tutku             #+#    #+#             */
/*   Updated: 2026/05/25 23:31:30 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"

void ServerSocket::closeSocket() //TODO: move to utils
{
	if (_fd != -1)
	{
		close(_fd);
		_fd = -1;
	}
}

int ServerSocket::setup(void)
{
	if (this->createSocket() == ERROR)
	{
		closeSocket();
		return ERROR;
	}
	if (this->setSocketOptions() == ERROR)
	{
		closeSocket();
		return ERROR;
	}
	if (this->setAddress() == ERROR)
	{
		closeSocket();
		return ERROR;
	}
	if (this->bindSocket() == ERROR)
	{
		closeSocket();
		return ERROR;
	}
	if (this->listenSocket() == ERROR)
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
int ServerSocket::setNonBlocking(int fd)
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
int ServerSocket::createSocket()
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == ERROR)
	{
		std::cerr << "Error opening socket: " << std::strerror(errno) << std::endl;
		return ERROR;
	}
	if (setNonBlocking(_fd) == ERROR)
		return ERROR;
	std::cout << "Socket created successfully!" << std::endl;
	return SUCCESS;
}

/*
 SO_REUSEADDR option for when you restart the server, dont get:
	Address already in use
*/
int ServerSocket::setSocketOptions()
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
int ServerSocket::setAddress()
{
	_address.sin_family = AF_INET;
	_address.sin_port = htons(_port);
	_address.sin_addr.s_addr = htonl(_host);

	return SUCCESS;
}

// finish this function by checking the documentation
// https://www.linuxhowtos.org/C_C++/socket.htm
int ServerSocket::bindSocket()
{
	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) == ERROR)
	{
		std::cerr << "Couldn't bind the port!: " << std::strerror(errno) << std::endl;
		return ERROR;
	}
	std::cout << "Bind successful" << std::endl;
	return SUCCESS;
}

int ServerSocket::listenSocket()
{
	if (listen(this->_fd, 128) == ERROR)
	{
		std::cerr << "Couldn't listen socket!: " << std::strerror(errno) << std::endl;
		return ERROR;
	}
	std::cout << "Listen successful" << std::endl;
	return SUCCESS;
}

ServerSocket::ServerSocket() : _fd(-1)
{
	ft_memset(&_address, 0, sizeof(_address));
}

ServerSocket::ServerSocket(uint32_t &host, int port) 
	: _fd(-1), _host(host), _port(port)
{
	ft_memset(&_address, 0, sizeof(_address));
}

// ServerSocket &ServerSocket::operator= (const ServerSocket &other)
// {
// 	if (this != &other)
// 	{
// 		this->_fd = other._fd;
// 		this->_address = other._address;
// 	}
// 	return (*this);
// }

// ServerSocket::ServerSocket(const ServerSocket &other)
// {
// 	*this = other;
// }

ServerSocket::~ServerSocket()
{
	closeSocket();
}

int ServerSocket::get_fd() const
{
	return (this->_fd);
}

struct sockaddr_in &ServerSocket::getAddress()
{
	return (this->_address);
}
