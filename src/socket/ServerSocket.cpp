/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 21:31:15 by tutku             #+#    #+#             */
/*   Updated: 2026/05/12 14:17:30 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"

int ServerSocket::setup(void)
{
	this->createSocket();
	this->bindSocket();
	this->listenSocket();
	return SUCCESS;
}

/*
	create a socket and make it non-blocking
	AF_INET : IPv4 protocol
	SOCK_STREAM: TCP socket
*/
int ServerSocket::createSocket()
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == ERROR)
	{
		std::cerr << "Error opening socket: " << std::strerror(errno) << std::endl;
		return ERROR;
	}
	int flags = fcntl(_fd, F_GETFL, 0);
	if (flags < 0)
	{
		std::cerr << "Error fcntl F_GETFL: " << std::strerror(errno) << std::endl;
		close(_fd); //TODO: check
		return (ERROR);
	}
	int status = fcntl(_fd, F_SETFL, flags | O_NONBLOCK);
	if (status == ERROR)
	{
		std::cerr << "Error making the socket non-blocking: " << std::strerror(errno) << std::endl;
		close(_fd); //TODO: check
		return ERROR;
	}
	std::cout << "Socket created successfully!" << std::endl;
	return SUCCESS;

}

// finish this function by checking the documentation
// https://www.linuxhowtos.org/C_C++/socket.htm
int ServerSocket::bindSocket()
{
	ft_memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_port = htons(8080); //TODO: change later when config is ready
	_address.sin_addr.s_addr = INADDR_ANY; // TODO: later change value, IP address of the host/server

	if (bind(this->get_fd(), (struct sockaddr *)&this->getAddress(), sizeof(this->getAddress())) == ERROR)
	{
		std::cerr << "Couldn't bind the port!: " << std::strerror(errno) << std::endl;
		return ERROR;
	}
	std::cout << "Bind successful" << std::endl;
	return SUCCESS;
}

int ServerSocket::listenSocket()
{
	if (listen(this->_fd, BACKLOG) == ERROR)
	{
		std::cerr << "Couldn't listen socket!: " << std::strerror(errno) << std::endl;
		return ERROR;
	}
	std::cout << "Listen successful" << std::endl;
	return SUCCESS;
}


ServerSocket::ServerSocket()
{
}

ServerSocket &ServerSocket::operator= (const ServerSocket &other)
{
	if (this != &other)
	{
		this->_fd = other._fd;
	}
	return (*this);
}

ServerSocket::ServerSocket(const ServerSocket &other)
{
	*this = other;
}

ServerSocket::~ServerSocket()
{
}

int ServerSocket::get_fd()
{
	return (this->_fd);
}

struct sockaddr_in &ServerSocket::getAddress()
{
	return (this->_address);
}
