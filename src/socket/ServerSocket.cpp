/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 21:31:15 by tutku             #+#    #+#             */
/*   Updated: 2026/05/10 22:47:07 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"

int ServerSocket::setup(void)
{
	this->createSocket();
	this->bindSocket();
	listen(this->get_fd(), 5);// TODO: change 5
}

/*
	AF_INET : IPv4 protocol
	SOCK_STREAM: TCP socket
*/
int ServerSocket::createSocket()
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
	{
		std::cerr << "Error opening socket" << std::endl;
		return ERROR;
	}
	return SUCCESS;

}

void *ft_memset(void *s, int c, size_t n) //TODO:move somewhere else
{
	unsigned char *ptr;
	size_t counter;

	ptr = (unsigned char *)s;
	counter = 0;
	while (counter < n)
	{
		ptr[counter] = (unsigned char)c;
		counter++;
	}
	return (s);
}

// finish this function by checking the documentation
// https://www.linuxhowtos.org/C_C++/socket.htm
int ServerSocket::bindSocket()
{
	ft_memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	// htons() which converts a port number in host byte order to a port number in network byte order.
	_address.sin_port = htons(8080); //TODO:change passed port value
	// s_addr will always be the IP address of the machine on which the server is running
	/*
	INADDR_ANY: It is used when we don't want to bind our socket 
		to any particular IP and instead make it listen to all the available IPs.

	*/
	_address.sin_addr.s_addr = INADDR_ANY; // TODO: change value, IP address of the host

	if (bind(this->get_fd(), (struct sockaddr *)&this->getAddress(), sizeof(this->getAddress())) == -1)
	{
		std::cerr << "Couldn't bind the port!" << std::endl;
		return ERROR;
	}
}

ServerSocket::ServerSocket()
{
}

ServerSocket::~ServerSocket()
{
}

ServerSocket &ServerSocket::operator = (const ServerSocket &other)
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

int ServerSocket::get_fd()
{
	return (this->_fd);
}

struct sockaddr_in ServerSocket::getAddress()
{
	return (this->_address);
}
